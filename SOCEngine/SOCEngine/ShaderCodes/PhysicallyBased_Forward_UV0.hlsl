#include "PhysicallyBased_Forward_Common.h"

struct VS_INPUT
{
	float3 position					: POSITION;
	float2 uv						: TEXCOORD0;
};

struct PS_SCENE_INPUT
{
	float4 position 	 			: SV_POSITION;
	float3 positionWorld			: POSITION_WORLD;
	float2 uv						: TEXCOORD0;
};

PS_SCENE_INPUT VS(VS_INPUT input)
{
	PS_SCENE_INPUT ps;
	float4 posWorld = mul(float4(input.position, 1.0f), transform_world);
	ps.positionWorld = posWorld.xyz;

	ps.position = mul(posWorld, cameraMat_viewProj);

	ps.uv = input.uv;

	return ps;
}

float4 PS(PS_SCENE_INPUT input) : SV_Target
{
	float3 normal	= float3(0.0f, 0.0f, 0.0f);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalTexture.Sample(defaultSampler, input.uv).a;
	return Lighting(normal, roughness, input.positionWorld, input.position.xy, input.uv);
#else
	return Lighting(normal, input.positionWorld, input.position.xy, input.uv);
#endif
}

#include "OptionalRendering_Forward.h"