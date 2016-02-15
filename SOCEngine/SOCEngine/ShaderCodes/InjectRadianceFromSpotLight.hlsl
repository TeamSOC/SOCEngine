//EMPTY_META_DATA

#include "Injection_Common.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

[numthreads(INJECTION_TILE_RES, INJECTION_TILE_RES, 1)]
void InjectRadianceSpotLightsCS(uint3 globalIdx	: SV_DispatchThreadID, 
								uint3 localIdx	: SV_GroupThreadID,
								uint3 groupIdx	: SV_GroupID)
{
	uint capacity			= GetNumOfSpotLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	float perShadowMapRes	= (float)(1 << GetNumOfSpotLight(shadowGlobalParam_packedPowerOfTwoShadowResolution));

	uint shadowIndex		= globalIdx.x / (uint)perShadowMapRes;
	ParsedShadowParam shadowParam;
	ParseShadowParam(shadowParam, g_inputSpotLightShadowParams[shadowIndex]);
	uint lightIndex			= shadowParam.lightIndex;

	float2 shadowMapPos	= float2(globalIdx.x % uint(perShadowMapRes), globalIdx.y % uint(perShadowMapRes));
	float2 shadowMapUV	= shadowMapPos.xy / perShadowMapRes;

	float depth = g_inputSpotLightShadowMapAtlas.Load(uint3(globalIdx.xy, 0), 0);

	float4 worldPos = mul( float4(shadowMapPos.xy, depth, 1.0f), g_inputSpotLightShadowInvVPVMatBuffer[shadowIndex].mat );
	worldPos /= worldPos.w;

	float3 voxelSpaceUV = (worldPos.xyz - voxelization_minPos) / voxelization_voxelizeSize;
	int dimension		= (int)GetDimension();
	int3 voxelIdx		= int3(voxelSpaceUV * dimension);

	if( any(voxelIdx < 0) || any(dimension <= voxelIdx) )
		return;

	//float4 screenSpaceCoord = float4( shadowMapUV.x * 2.0f - 1.0f,
	//								-(shadowMapUV.y * 2.0f - 1.0f),
	//								depth, 1.0f );

	//float4 voxelSpaceUV = mul(screenSpaceCoord, injection_volumeProj);
	//voxelSpaceUV /= voxelSpaceUV.w;
	//voxelSpaceUV.xyz = voxelSpaceUV.xyz * 0.5f + 0.5f;
	//uint3 voxelIdx = uint3(voxelSpaceUV.xyz * dimension);

	float4 lightCenterWithRadius	= g_inputSpotLightTransformBuffer[lightIndex];
	float3 lightCenterWorldPos		= lightCenterWithRadius.xyz;
	float radiusWithMinusZDirBit	= lightCenterWithRadius.a;

	float4 spotParam	= g_inputSpotLightParamBuffer[lightIndex];
	float3 lightDir		= float3(spotParam.x, spotParam.y, 0.0f);
	lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
	lightDir.z = (radiusWithMinusZDirBit >= 0.0f) ? lightDir.z : -lightDir.z;

	float radius = abs(radiusWithMinusZDirBit);

	float outerCosineConeAngle	= spotParam.z;
	float innerCosineConeAngle	= spotParam.w;

	float3 vtxToLight				= lightCenterWorldPos - worldPos.xyz;
	float3 vtxToLightDir			= normalize(vtxToLight);
	float distanceOfLightWithVertex = length(vtxToLight);
	float currentCosineConeAngle	= dot(-vtxToLightDir, lightDir);

	float4 albedo	= GetColor(g_inputAnistropicVoxelAlbedoTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float3 normal	= GetNormal(g_inputAnistropicVoxelNormalTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float4 emission	= GetColor(g_inputAnistropicVoxelEmissionTexture, voxelIdx, lightDir, voxelization_currentCascade);

	float3 radiosity = float3(0.0f, 0.0f, 0.0f);
	if( (distanceOfLightWithVertex < (radius * 1.5f)) &&
		(outerCosineConeAngle < currentCosineConeAngle) )
	{
		float innerOuterAttenuation = saturate( (currentCosineConeAngle - outerCosineConeAngle) / (innerCosineConeAngle - outerCosineConeAngle));
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
		innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
		innerOuterAttenuation = lerp(innerOuterAttenuation, 1, innerCosineConeAngle < currentCosineConeAngle);

		float4 lightColorWithLm = g_inputPointLightColorBuffer[lightIndex];
		float lumen = lightColorWithLm.w * 12750.0f; //maximum lumen is 12750.0f

		float plAttenuation = 1.0f / (distanceOfLightWithVertex * distanceOfLightWithVertex);
		float totalAttenTerm = lumen * plAttenuation * innerOuterAttenuation;

		float3 lightColor = lightColorWithLm.rgb;
		float3 lambert = albedo.rgb * saturate(dot(normal, lightDir));

		radiosity = lambert * totalAttenTerm * lightColor * RenderSpotLightShadow(lightIndex, worldPos.xyz, distanceOfLightWithVertex / radius);
	}
	radiosity += emission.rgb;

	StoreRadiosity(radiosity, albedo.a, normal, voxelIdx);
}

#endif