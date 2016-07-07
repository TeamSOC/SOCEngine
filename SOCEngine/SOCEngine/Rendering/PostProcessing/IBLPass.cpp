#include "IBLPass.h"
#include "Director.h"
#include "BindIndexInfo.h"
#include "ResourceManager.h"
#include "SkyBox.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::TBDR;
using namespace Rendering::Sky;
using namespace Device;
using namespace Resource;

IBLPass::IBLPass() : FullScreen()
{
}

IBLPass::~IBLPass()
{
}

void IBLPass::Initialize()
{
	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());
	FullScreen::Initialize("IBLPass", "PS", &macros);
	
	const Texture2D* preIntegrateEnvBRDFMap = ResourceManager::SharedInstance()->GetPreIntegrateEnvBRDFMap();
	_inputTextures.push_back(ShaderForm::InputTexture(uint(TextureBindIndex::IBLPass_PreIntegrateEnvBRDFMap), preIntegrateEnvBRDFMap, false, false, false, true));
}

void IBLPass::Render(const Device::DirectX* dx, const RenderTexture* outResultRT, const MeshCamera* meshCam, const SkyForm* sky)
{
	auto BindTexturesToPixelShader = [](ID3D11DeviceContext* context, TextureBindIndex bind, const Texture2D* tex)
	{
		ID3D11ShaderResourceView* srv = tex ? tex->GetShaderResourceView()->GetView() : nullptr;
		context->PSSetShaderResources(uint(bind), 1, &srv);
	};

	ID3D11DeviceContext* context	= dx->GetContext();

	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Albedo_Occlusion,					meshCam->GetGBufferAlbedoOcclusion());
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Emission_MaterialFlag,				meshCam->GetGBufferEmissionMaterialFlag());
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_MotionXY_Metallic_Specularity,		meshCam->GetGBufferMotionXYMetallicSpecularity());
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Normal_Roughness,					meshCam->GetGBufferNormalRoughness());	
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Depth,								meshCam->GetOpaqueDepthBuffer());
	BindTexturesToPixelShader(context, TextureBindIndex::IBLPass_IlluminationMap,					meshCam->GetRenderTarget());

	ID3D11Buffer* buffer = meshCam->GetTBRParamConstBuffer()->GetBuffer();
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::TBRParam), 1, &buffer);

	// Sky Cube Map
	if(sky)
	{
		const Texture2D* cubeMap = nullptr;

		if(sky->GetType() == SkyForm::Type::Box)
			cubeMap = dynamic_cast<const SkyBox*>(sky)->GetSkyCubeMap();
		else
			ASSERT_MSG("cant support");

		BindTexturesToPixelShader(context, TextureBindIndex::AmbientCubeMap, cubeMap);

		buffer = sky->GetSkyMapInfoConstBuffer()->GetBuffer();
		context->PSSetConstantBuffers(uint(ConstBufferBindIndex::SkyMapInfoParam), 1, &buffer);
	}

	ID3D11SamplerState* sampler		= dx->GetSamplerStateLinear();
	context->PSSetSamplers(uint(SamplerStateBindIndex::AmbientCubeMapSamplerState), 1, &sampler);

	FullScreen::Render(dx, outResultRT);

	sampler = nullptr;
	context->PSSetSamplers(uint(SamplerStateBindIndex::AmbientCubeMapSamplerState), 1, &sampler);
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::SkyMapInfoParam), 1, &buffer);

	buffer = nullptr;
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::TBRParam), 1, &buffer);

	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Albedo_Occlusion,			nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Emission_MaterialFlag,		nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_MotionXY_Metallic_Specularity,	nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Normal_Roughness,			nullptr);	
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Depth,						nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::AmbientCubeMap,					nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::IBLPass_IlluminationMap,			nullptr);
}