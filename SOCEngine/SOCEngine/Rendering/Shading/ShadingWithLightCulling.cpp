#include "ShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Rendering::DeferredShading;

ShadingWithLightCulling::ShadingWithLightCulling() : 
	_offScreen(nullptr), _inputPointLightColorBuffer(nullptr),
	_inputSpotLightColorBuffer(nullptr), _inputDirectionalLightColorBuffer(nullptr),
	_inputDirectionalLightParamBuffer(nullptr), _inputDirectionalLightTransformBuffer(nullptr)
{
}

ShadingWithLightCulling::~ShadingWithLightCulling()
{
	Destory();
}

void ShadingWithLightCulling::Initialize(const Texture::DepthBuffer* opaqueDepthBuffer,
												 const Texture::RenderTexture* gbuffer_albedo_metallic,  
												 const Texture::RenderTexture* gbuffer_specular_fresnel0, 
												 const Texture::RenderTexture* gbuffer_normal_roughness, 
												 const Math::Size<uint>& backBufferSize)
{
	Manager::LightManager* lightManager = Director::GetInstance()->GetCurrentScene()->GetLightManager();

	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "TileBasedDeferredShading");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	LightCulling::Initialize(filePath, "CS", opaqueDepthBuffer, nullptr, RenderType::TBDR);

	// Input buffer
	{
		// Point Light Color
		{
			uint idx = (uint)InputBufferShaderIndex::PointLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetPointLightColorBufferSR();
			AddInputBufferToList(_inputPointLightColorBuffer, idx, srBuffer);
		}

		// Spot Light Color
		{
			uint idx = (uint)InputBufferShaderIndex::SpotLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetSpotLightColorBufferSR();
			AddInputBufferToList(_inputSpotLightColorBuffer, idx, srBuffer);
		}

		// Directional Light
		{
			// Center With DirZ
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightCenterWithDirZ;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightTransformBufferSR();
				AddInputBufferToList(_inputDirectionalLightTransformBuffer, idx, srBuffer);
			}

			// Color
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightColor;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightColorBufferSR();
				AddInputBufferToList(_inputSpotLightColorBuffer, idx, srBuffer);
			}

			// Param half / DirX, DirY
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightParam;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightParamBufferSR();
				AddInputBufferToList(_inputDirectionalLightParamBuffer, idx, srBuffer);
			}
		}
	}

	// Input Texture
	{
		// Albedo_Metallic
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Albedo_Metallic;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_albedo_metallic);
		}

		// Specular_Fresnel0
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Specular_Fresnel0;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_specular_fresnel0);
		}

		// Normal_Roughness
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Normal_Roughness;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_normal_roughness);
		}
	}

	// Offscreen
	{
		Math::Size<uint> bufferSize = backBufferSize;
		{
			const DXGI_SAMPLE_DESC& msaaDesc = Director::GetInstance()->GetDirectX()->GetMSAADesc();
			if(msaaDesc.Count > 1)
			{
				bufferSize.w *= 2;
				bufferSize.h *= 2;
			}
		}

		_offScreen = new CSRWTexture;
		_offScreen->Initialize(bufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 0);

		ComputeShader::Output output;
		{
			output.idx		= (uint)OutputBufferShaderIndex::OutScreen;
			output.output	= _offScreen;
		}

		std::vector<ComputeShader::Output> outputs;
		outputs.push_back(output);

		SetOuputBuferToCS(outputs);
	}

	SetInputsToCS();
}

void ShadingWithLightCulling::Destory()
{
	_inputPointLightColorBuffer				= nullptr;
	_inputSpotLightColorBuffer				= nullptr;
	_inputDirectionalLightTransformBuffer	= nullptr;
	_inputDirectionalLightColorBuffer		= nullptr;
	_inputDirectionalLightParamBuffer		= nullptr;

	SAFE_DELETE(_offScreen);
	LightCulling::Destroy();
}