#include "PhysicallyBasedMaterial.h"
#include "Utility.h"
#include "Director.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;

PhysicallyBasedMaterial::GBufferParam::GBufferParam()
{
	mainColor.r = mainColor.g = mainColor.b = 1.0f;
	metallic = roughness = fresnel0 = 0.0f;
	uvTiling.x = uvTiling.y = 1.0f;
}

PhysicallyBasedMaterial::GBufferParam::~GBufferParam(){}

PhysicallyBasedMaterial::PhysicallyBasedMaterial(const std::string& name)
	: Material(name, Type::PhysicallyBasedModel), 
	_gbufferCB(nullptr), _constBufferUpdateCounter(0)
{
}

PhysicallyBasedMaterial::~PhysicallyBasedMaterial(void)
{
	Destroy();
}

void PhysicallyBasedMaterial::Initialize()
{
	ASSERT_COND_MSG(_gbufferCB == nullptr, "Error, gbuffer const buffer was already allocated");

	_gbufferCB = new ConstBuffer;
	_gbufferCB->Initialize(sizeof(GBufferParam));
}

void PhysicallyBasedMaterial::Destroy()
{
	SAFE_DELETE(_gbufferCB);
}

void PhysicallyBasedMaterial::UpdateConstBuffer(const Device::DirectX* dx)
{
	if( _constBufferUpdateCounter < GetVariableUpdateCounter() )
	{
		GBufferParam param;
		GetMainColor(param.mainColor);
		GetRoughness(param.roughness);
		GetFresnel_0(param.fresnel0);
		GetMetallic(param.metallic);
		GetUVTiling(param.uvTiling);

		_gbufferCB->UpdateSubResource(dx->GetContext(), &param);
		_constBufferUpdateCounter = GetVariableUpdateCounter();

		uint idx = (uint)InputConstBufferShaderIndex::Material;
		SetConstBufferUseShaderSlotIndex(idx, _gbufferCB, BaseShader::Usage(false, false, false, true));
	}
}

void PhysicallyBasedMaterial::UpdateMainColor(const Color& color)
{
	GBufferParam::MainColor mainColor;
	{
		mainColor.r = color.r;
		mainColor.g = color.g;
		mainColor.b = color.b;
	}
	SetVariable("MainColor", mainColor);

	_hasAlpha = (color.a < 1.0f);
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateDiffuseMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Diffuse, tex, BaseShader::Usage(false, false, false, true));
	_hasAlpha = tex->GetHasAlpha();
	_changedAlpha = true;
}

void PhysicallyBasedMaterial::UpdateNormalMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Normal, tex, BaseShader::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateSpecularMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Specular, tex, BaseShader::Usage(false, false, false, true));
}

void PhysicallyBasedMaterial::UpdateOpacityMap(const Rendering::Texture::Texture2D* tex)
{
	SetTextureUseShaderSlotIndex( (uint)InputTextureShaderIndex::Opacity, tex, BaseShader::Usage(false, false, false, true));

	_hasAlpha = tex != nullptr;
	_changedAlpha = true;
}