//NOT_CREATE_META_DATA

#ifndef __SOC_PHYSICALLY_BASED_COMMON_H__
#define __SOC_PHYSICALLY_BASED_COMMON_H__

#if (MSAA_SAMPLES_COUNT > 1)
#define ALPHA_TEST_BIAS		0.003f
#else
#define ALPHA_TEST_BIAS		0.5f
#endif

cbuffer Transform : register( b1 )		//Mesh
{
	matrix transform_world;
	matrix transform_worldView;
	matrix transform_worldViewProj;
};

cbuffer Material : register( b2 )		//PhysicallyBasedMaterial
{
	float3	material_mainColor;
	uint	material_alpha_metallic_roughness_emission;

	float2 	material_uvTiling0;
	float2 	material_uvOffset0;
	float2 	material_uvTiling1;
	float2 	material_uvOffset1;
};

Texture2D diffuseTexture			: register( t8 );
Texture2D normalTexture				: register( t9 );
Texture2D specularTexture			: register( t10 );
Texture2D opacityTexture			: register( t11 ); // 0 is opcity 100%, 1 is 0%. used in Transparency Rendering

float3 NormalMapping(float3 normalMapXYZ, float3 normal, float3 tangent, float2 uv)
{
	float3 binormal = normalize( cross(normal, tangent) );

	float3 texNormal = normalMapXYZ;
	texNormal *= 2.0f; texNormal -= float3(1.0f, 1.0f, 1.0f);

	float3x3 TBN = float3x3(normalize(binormal), normalize(tangent), normalize(normal));

	return normalize( mul(texNormal, TBN) );
}

void Parse_Metallic_Roughness_Emission(out float metallic,
									   out float roughness,
									   out float emission)
{
	uint scaledMetallic		= (material_alpha_metallic_roughness_emission & 0x00ff0000) >> 16;
	uint scaledRoughness	= (material_alpha_metallic_roughness_emission & 0x0000ff00) >> 8;
	uint scaledEmission		= (material_alpha_metallic_roughness_emission & 0x000000ff) >> 0;

	metallic	= (float)scaledMetallic		/ 255.0f;
	roughness	= (float)scaledRoughness	/ 255.0f;
	emission	= (float)scaledEmission		/ 255.0f;
}

float ParseMaterialAlpha()
{
	return ( (float)((material_alpha_metallic_roughness_emission & 0xff000000) >> 24) / 255.0f );
}

bool HasDiffuseTexture()
{
	return material_mainColor.r < 0.0f;
}

bool HasNormalTexture()
{
	return material_mainColor.g < 0.0f;
}

bool HasSpecularTexture()
{
	return material_mainColor.b < 0.0f;
}

#endif