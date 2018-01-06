//EMPTY_META_DATA

#define USE_TBR_PARAM
#include "FullScreenShader.h"
#include "TBDRInput.h"
#include "CommonConstBuffer.h"

cbuffer MotionBlurParamCB : register(b1)
{
	float	Length;
	float	KernelSize;
	float	KernelStep;
	uint	dummy;
};

Texture2D<float4>	InputSceneMap			: register( t0 );
SamplerState		PointSampler			: register( s0 );
SamplerState		LinearSampler			: register( s1 );

float2 GetVelocity(float2 uv, uniform int2 offset)
{
#if (MSAA_SAMPLES_COUNT > 1)
	return GBufferVelocity_metallic_specularity.Load(GetViewportSize() * uv + offset, 0).rg;
#else
	return GBufferVelocity_metallic_specularity.Sample(PointSampler, uv, offset).rg;
#endif
}

#define SAMPLE_COUNT 32
float4 MotionBlurPS(PS_INPUT input) : SV_Target
{
	const int Offset = 2;
	float2 tl = GetVelocity(input.uv, int2(-Offset, -Offset));
	float2 tr = GetVelocity(input.uv, int2( Offset, -Offset));
	float2 bl = GetVelocity(input.uv, int2(-Offset,  Offset));
	float2 br = GetVelocity(input.uv, int2( Offset,  Offset));
	float2 ce = GetVelocity(input.uv, int2(      0,       0));
	float2 velocity = (tl + tr + bl + br + ce) * 0.2f * Length;

	float4 color = 0;
	for(float i=-KernelSize; i<=KernelSize; i+=KernelStep)
	{
		float2 uv = saturate(input.uv + velocity * i);
		color += InputSceneMap.Sample(LinearSampler, uv);
	}

	return color / color.a;
}