#include "CommonConstBuffer.h"
#include "TBDRInput.h"
#include "ToneMapping.h"

cbuffer LightProbeParam : register( b0 )
{
	matrix	lpParam_viewProjs[6];
	matrix	lpParam_worldMat;
};

cbuffer SkyScatteringParam : register( b3 )
{
	uint	ssParam_rayleighWithTurbidity;
	uint	ssParam_directionalLightIndexWithLuminance;
	uint	ssParam_mieCoefficientWithDirectionalG;

	float	ssParam_sunFade;
	float	ssParam_sunIntensity;
	float3	ssParam_sunWorldPos;
};

float GetRayleigh()
{
	return f16tof32(ssParam_rayleighWithTurbidity & 0xffff);
}
float GetTurbidity()
{
	return f16tof32(ssParam_rayleighWithTurbidity >> 16);
}
uint GetDirectionalLightIndex()
{
	return ssParam_directionalLightIndexWithLuminance & 0xffff;
}
float GetLuminance()
{
	return f16tof32(ssParam_directionalLightIndexWithLuminance >> 16);
}
float GetMieCoefficient()
{
	return f16tof32(ssParam_mieCoefficientWithDirectionalG & 0xffff);
}
float GetMieDirectionalG()
{
	return f16tof32(ssParam_mieCoefficientWithDirectionalG >> 16);
}
float GetSunFade()
{
	return ssParam_sunFade;
}
float GetSunIntensity()
{
	return ssParam_sunIntensity;
}
float3 GetSunWorldPosition()
{
	return ssParam_sunWorldPos;
}

float3 ComputeTotalMie(float3 lambda, float T)
{
	// mie stuff
	// K coefficient for the primaries
	const float v	= 4.0f;
	const float3 K	= float3(0.686f, 0.678f, 0.666f);

	float c = (0.2f * T ) * 10E-18f;
	return 0.434f * c * PI * pow((2.0f * PI) / lambda, v - 2.0f) * K;
}

float3 GetLightDir()
{
	uint lightIndex = GetDirectionalLightIndex();
	float3 lightDir = float3(DirectionalLightDirXYBuffer[lightIndex], 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y) * GetSignDirectionalLightDirZSign(DirectionalLightOptionalParamIndex[lightIndex]);

	return -lightDir;
}

struct VS_INPUT
{
	float3 position 			: POSITION;
};

struct VS_OUTPUT
{
	float4 localPos				: LOCAL_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.localPos = float4(input.position, 1.0f);

	return output;
}

struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float3 localPos				: LOCAL_POS;

	uint rtIndex				: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<PS_INPUT> stream)
{
	for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
	{
		PS_INPUT output;
		output.rtIndex = faceIdx;

		for(uint i=0; i<3; ++i)
		{
			output.position = mul(input[i].localPos, lpParam_viewProjs[faceIdx]);
			output.localPos = input[i].localPos.xyz;

			stream.Append(output);
		}
		stream.RestartStrip();
	}
}

float rayleighPhase(float cosTheta)
{
	return (3.0f / (16.0f * PI)) * (1.0f + pow(cosTheta, 2.0f));
}

float hgPhase(float cosTheta, float g)
{
	return (1.0f / (4.0f * PI)) * ((1.0f - pow(g, 2.0f)) / pow(1.0f - 2.0f * g * cosTheta + pow(g, 2.0f), 1.5f));
}

float4 PS(PS_INPUT input) : SV_Target
{		
	float rayleighCoefficient = GetRayleigh() - (1.0f * (1.0f - GetSunFade()));
	// wavelength of used primaries, according to preetham
	const float3 lambda = float3(680E-9f, 550E-9f, 450E-9f);

	// mie coefficients
	float3 betaM = ComputeTotalMie(lambda, GetTurbidity()) * GetMieCoefficient();

	// see http://blenderartists.org/forum/showthread.php?321110-Shaders-and-Skybox-madness
	// A simplied version of the total Reayleigh scattering to works on browsers that use ANGLE
	const float3 simplifiedRayleigh = 0.0005f / float3(94, 40, 18);

	// extinction (absorbtion + out scattering)
	// rayleigh coefficients
	float3 betaR = simplifiedRayleigh * rayleighCoefficient;

	// optical length at zenith for molecules
	const float rayleighZenithLength	= 8.4E3f;
	const float mieZenithLength			= 1.25E3f;

	const float sunAngularDiameterCos	= 0.9999f;

	// Scattering
	float3 result = float3(0.0f, 0.0f, 0.0f);
	{
		float3 ToCamDir		= normalize(input.localPos);

		// optical length
		// cutoff angle at 90 to avoid singularity in next formula.
		float zenithAngle	= acos( max(0.0, dot(float3(0.0f, 1.0f, 0.0f), ToCamDir)) );
		float norm			= cos(zenithAngle) + 0.15f * pow(93.885f - RAD_2_DEG(zenithAngle), -1.253f);
		float sR			= rayleighZenithLength	/ norm;
		float sM			= mieZenithLength		/ norm;

		float3 lightDir		= GetLightDir();

		// in scattering
		float cosTheta		= dot(ToCamDir, lightDir);

		float rPhase		= rayleighPhase(cosTheta * 0.5f + 0.5f);
		float3 betaRTheta	= betaR * rPhase;

		float mPhase		= hgPhase(cosTheta, GetMieDirectionalG());
		float3 betaMTheta	= betaM * mPhase;

		// combined extinction factor
		float3 Fex			= exp( -(betaR * sR + betaM * sM) );
		float3 x			= ((betaRTheta + betaMTheta) / (betaR + betaM));

		float3	Lin			=	pow(GetSunIntensity() * x * (1.0f - Fex), 1.5f);
				Lin			*=	lerp(float3(1.0f, 1.0f, 1.0f),
									pow(GetSunIntensity() * x * Fex, 1.0f / 2.0f),
									saturate( pow(1.0f - dot(float3(0.0f, 1.0f, 0.0f), lightDir), 5.0f) ));
		
		float3 L0			= Fex * 0.001f;

		// composition + solar disc
		float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.00002f, cosTheta);
		L0 += (GetSunIntensity() * 500.0f * Fex) * sundisk;

		float3 texColor = (Lin + L0) * 0.04f + float3(0.0f, 0.0003f, 0.00075f);

		float3 curr				= Uncharted2ToneMapping((log2(2.0f / pow(GetLuminance(), 4.0f))) * texColor);
		const float whiteScale	= 1.0748724675633854f;
		float3 color			= curr * whiteScale;

		float coff = 1.0f / (1.2f + (1.2f * GetSunFade()));
		result.rgb = pow(color, coff);
	}

	return float4(result.rgb, 1.0f);
}
