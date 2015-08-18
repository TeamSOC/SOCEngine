#include "PhysicallyBased_Transparency_Common.h"

struct VS_INPUT
{
	float4 position					: POSITION;
	float2 uv						: TEXCOORD0;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;
	float2 uv						: TEXCOORD0;
};

struct PS_POSITION_ONLY_INPUT //used in writing depth buffer
{
	float4 position 	 			: SV_POSITION;
};

/******************** Render Scene ********************/

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;
	ps.positionWorld	= mul(input.position, transform_world).xyz;
	ps.position			= mul(input.position, transform_worldViewProj);
	ps.uv				= input.uv;

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float4	diffuseTex		= diffuseTexture.Sample(transparencyDefaultSampler, input.uv);
	float3	diffuseColor	= diffuseTex.rgb;

	float3 normal	= float3(0.0f, 0.0f, 0.0f);
	float3 viewDir	= normalize( g_cameraWorldPosition - input.positionWorld );

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= DecodeNormalTexture(normalTexture, input.uv, transparencyDefaultSampler);
	lightParams.fresnel0		= material_fresnel0;
	lightParams.roughness		= material_roughness;
	lightParams.diffuseColor	= diffuseColor;
	lightParams.specularColor	= specularTexture.Sample(transparencyDefaultSampler, input.uv).rgb;

	uint tileIdx				= GetTileIndex(input.position.xy);
	uint startIdx				= tileIdx * g_maxNumOfperLightInTile + 1;

	uint packedLightCountValue	= g_perLightIndicesInTile[startIdx - 1];
	uint pointLightCount		= packedLightCountValue & 0x0000ffff;
	uint spotLightCount			= packedLightCountValue >> 16;

	float3 accumulativeFrontFaceDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeFrontFaceSpecular	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeBackFaceDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeBackFaceSpecular		= float3(0.0f, 0.0f, 0.0f);

	uint endIdx = startIdx + pointLightCount;
	for(uint pointLightIdx=startIdx; pointLightIdx<endIdx; ++pointLightIdx)
	{
		lightParams.lightIndex = g_perLightIndicesInTile[pointLightIdx];

		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderPointLightTwoSided(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			lightParams, input.positionWorld);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
	}

	startIdx += pointLightCount;
	endIdx += spotLightCount;
	for(uint spotLightIdx=startIdx; spotLightIdx<endIdx; ++spotLightIdx)
	{
		lightParams.lightIndex = g_perLightIndicesInTile[spotLightIdx];

		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderSpotLightTwoSided(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			lightParams, input.positionWorld);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
	}

	for(uint directionalLightIdx=0; directionalLightIdx<g_directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

		float3 frontFaceDiffuseColor, frontFaceSpecularColor;
		float3 backFaceDiffuseColor, backFaceSpecularColor;

		RenderDirectionalLightTwoSided(
			frontFaceDiffuseColor, frontFaceSpecularColor,
			backFaceDiffuseColor, backFaceSpecularColor,
			lightParams);

		accumulativeFrontFaceDiffuse	+= frontFaceDiffuseColor;
		accumulativeFrontFaceSpecular	+= frontFaceSpecularColor;
		accumulativeBackFaceDiffuse		+= backFaceDiffuseColor;
		accumulativeBackFaceSpecular	+= backFaceSpecularColor;
	}

	float3	result = accumulativeFrontFaceDiffuse + accumulativeFrontFaceSpecular + ( TRANSPARENCY_BACK_FACE_WEIGHT * (accumulativeBackFaceDiffuse + accumulativeBackFaceSpecular) );
	float	alpha = diffuseTex.a * opacityTexture.Sample(transparencyDefaultSampler, input.uv).x;

	return float4(result, alpha);
}

/******************** Only Position, Only Write DepthBuffer ********************/

PS_POSITION_ONLY_INPUT PositionOnlyVS(VS_INPUT input)
{
	PS_POSITION_ONLY_INPUT ps;

	ps.position = mul(input.position, transform_worldViewProj);

	return ps;
}