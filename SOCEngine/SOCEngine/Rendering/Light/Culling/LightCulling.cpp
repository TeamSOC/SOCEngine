#include "LightCulling.h"
#include "Director.h"

#include "ResourceManager.h"

#include "Utility.h"
#include "EngineShaderFactory.hpp"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace GPGPU::DirectCompute;
using namespace Resource;

LightCulling::LightCulling() : _computeShader(nullptr), _globalDataBuffer(nullptr), _lightIndexBuffer(nullptr)
{

}

LightCulling::~LightCulling()
{
	Destroy();
}

void LightCulling::Initialize(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh)
{
	//혹시 모르니, 한번 초기화
	Destroy();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	std::string fileName, fileExtension, folderDir;
	bool valid = Utility::String::ParseDirectory(filePath, folderDir, fileName, fileExtension);
	ASSERT_COND_MSG(valid, "Where is your file extension?");

	std::string includeCode;
	{
		std::string macroCode = useRenderBlendedMesh ? "#define ENABLE_BLEND\n" : "";

		const DXGI_SAMPLE_DESC& desc = Device::Director::GetInstance()->GetDirectX()->GetMSAADesc();
		{
			macroCode += "#define MSAA_SAMPLES_COUNT ";
			macroCode += (desc.Count > 0) ? std::to_string(desc.Count) : "0";
			macroCode += "\n";
		}

		includeCode += macroCode;
		
		Factory::EngineFactory factory(nullptr); //FetchShaderFullPath만 사용

		std::string includeFilePath;
		factory.FetchShaderFullPath(includeFilePath, "LightCulling");
		ASSERT_COND_MSG(includeFilePath.empty() == false, "Error, includeFilePath is empty");

		bool success = shaderMgr->LoadShaderCode(includeCode, includeFilePath, false);
		ASSERT_COND_MSG( (includeCode.empty() == false) || (success == false), "Error, LightCulling Core Code does not exist");

		includeCode.insert(0, macroCode);
	}

	ID3DBlob* blob = shaderMgr->CreateBlob(folderDir, fileName, 
		"cs", mainFunc, false, &includeCode);

	ComputeShader::ThreadGroup threadGroup;
	UpdateThreadGroup(&threadGroup, false);
	_computeShader = new ComputeShader(threadGroup, blob);

	ASSERT_COND_MSG(_computeShader->Initialize(), "can not create compute shader");

	_globalDataBuffer = new ConstBuffer;
	ASSERT_COND_MSG(_globalDataBuffer->Initialize(sizeof(CullingConstBuffer)), "can not create const buffer");

	// Input Buffer Setting
	{
		ComputeShader::InputBuffer inputBufferElement;

		// Point Light
		{
			CSInputBuffer* pointLightCenterWithRadius = new CSInputBuffer;
			ASSERT_COND_MSG(pointLightCenterWithRadius->Initialize(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM), "cant create cs input buffer");
			inputBufferElement.idx = (uint)InputBuffer::PointLightRadiusWithCenter;
			inputBufferElement.buffer = pointLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// Spot Light
		{
			CSInputBuffer* spotLightCenterWithRadius = new CSInputBuffer;
			ASSERT_COND_MSG(spotLightCenterWithRadius->Initialize(sizeof(Math::Vector4), POINT_LIGHT_LIMIT_NUM), "cant create cs input buffer");
			inputBufferElement.idx = (uint)InputBuffer::SpotLightRadiusWithCenter;
			inputBufferElement.buffer = spotLightCenterWithRadius;
			_inputBuffers.push_back(inputBufferElement);
		}

		// depth buffer
		{
			// Opaque
			{
				ComputeShader::InputTexture inputTex;
				inputTex.idx		= (uint)InputTexture::InvetedOpaqueDepthBuffer;
				_inputTextures.push_back(inputTex);
			}

			// Blended, Transparent
			if(useRenderBlendedMesh)
			{
				ComputeShader::InputTexture inputTex;
				inputTex.idx		= (uint)InputTexture::InvetedBlendedDepthBuffer;
				_inputTextures.push_back(inputTex);
			}
		}

		_computeShader->SetInputBuffers(_inputBuffers);
		_computeShader->SetInputTextures(_inputTextures);
	}

	_useBlendedMeshCulling = useRenderBlendedMesh;
}

void LightCulling::InitializeOnlyLightCulling(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh)
{
	Initialize(filePath, mainFunc, useRenderBlendedMesh);

	// Ouput Buffer Setting
	{
		LightCulling_CSOutputBuffer* lightIndexBuffer = new LightCulling_CSOutputBuffer;
		Math::Size<unsigned int> size = CalcThreadSize();
		lightIndexBuffer->Initialize(size, CalcMaxNumLightsInTile());

		ComputeShader::OutputBuffer outputBuffer;
		{
			outputBuffer.idx = (uint)OutputBuffer::LightIndexBuffer;
			outputBuffer.buffer = lightIndexBuffer;
		}

		_outputBuffers.push_back(outputBuffer);
		_computeShader->SetOutputBuffers(_outputBuffers);
	}
}

unsigned int LightCulling::CalcMaxNumLightsInTile()
{
	const Math::Size<unsigned int>& winSize = Director::GetInstance()->GetWindowSize();
	const unsigned key = 16;

	return ( LightMaxNumInTile - ( key * ( winSize.h / 120 ) ) );
}

void LightCulling::UpdateInputBuffer(const Device::DirectX* dx,
									 const CullingConstBuffer& cbData,
									 const Light::LightForm::LightTransformBuffer* pointLightTransformBuffer,
									 const Light::LightForm::LightTransformBuffer* spotLightTransformBuffer)
{
	ID3D11DeviceContext* context = dx->GetContext();
	_globalDataBuffer->Update(context, &cbData);

	ComputeShader::InputBuffer inputBuffer;

	// Input Buffer Setting
	{
		//0, pointLight 
		_inputBuffers[0].buffer->Update(context, pointLightTransformBuffer);

		//1, spot light
		_inputBuffers[1].buffer->Update(context, spotLightTransformBuffer);
	}
}

void LightCulling::Dispatch(const Device::DirectX* dx, const Texture::DepthBuffer* invertedDepthBuffer, const Texture::DepthBuffer* invertedBlendedDepthBuffer)
{
	ID3D11DeviceContext* context = dx->GetContext();

	_inputTextures[0].texture = invertedDepthBuffer;

	if(_useBlendedMeshCulling)
		_inputTextures[1].texture = invertedBlendedDepthBuffer;

	_computeShader->Dispatch(context);
	
	_inputTextures[0].texture = nullptr;
	_inputTextures[1].texture = nullptr;
}

void LightCulling::UpdateThreadGroup(ComputeShader::ThreadGroup* outThreadGroup, bool updateComputeShader)
{
	Math::Size<unsigned int> threadSize = CalcThreadSize();
	ComputeShader::ThreadGroup threadGroup = ComputeShader::ThreadGroup(threadSize.w, threadSize.h, 1);

	if(outThreadGroup)
		(*outThreadGroup) = threadGroup;

	if(updateComputeShader)
		_computeShader->SetThreadGroupInfo(threadGroup);
}

const Math::Size<unsigned int> LightCulling::CalcThreadSize()
{
	auto CalcThreadLength = [](unsigned int size)
	{
		return (unsigned int)((size+TileSize-1) / (float)TileSize);
	};

	const Math::Size<unsigned int>& winSize = Director::GetInstance()->GetWindowSize();

	unsigned int width	= CalcThreadLength(winSize.w);
	unsigned int height = CalcThreadLength(winSize.h);

	return Math::Size<unsigned int>(width, height);
}

void LightCulling::Destroy()
{
	SAFE_DELETE(_computeShader);
	SAFE_DELETE(_globalDataBuffer);
}