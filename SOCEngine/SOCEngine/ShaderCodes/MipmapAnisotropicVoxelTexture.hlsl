//EMPTY_META_DATA

#include "ShaderCommon.h"

cbuffer MipMapInfoCB : register(b0)
{
	uint mipmapInfo_sourceDimension;
	uint3 mipmapInfo_dummy;
};

RWTexture3D<uint> VoxelMap		: register(u0);
RWTexture3D<uint> OutMipMap		: register(u1);

float4 AlphaBlending(float4 front, float4 back)
{
	return front + back * (1.0f - front.a);
}

uint AlphaBledningAnisotropicVoxelMap
	(	float4 front0,	float4 front1,	float4 front2,	float4 front3,
		float4 back0,	float4 back1,	float4 back2,	float4 back3	)
{
	float4 color0 = AlphaBlending(front0, back0);
	float4 color1 = AlphaBlending(front1, back1);
	float4 color2 = AlphaBlending(front2, back2);
	float4 color3 = AlphaBlending(front3, back3);

	return Float4ColorToUint( (color0 + color1 + color2 + color3) * 0.25f );
}

float4 GetColorFromVoxelMap(uint3 voxelIdx, uniform uint faceIndex)
{
#ifndef BASE_MIP_MAP
	voxelIdx.x += (faceIndex * mipmapInfo_sourceDimension);
#endif
	return RGBA8UintColorToFloat4(VoxelMap[voxelIdx]);
}

[numthreads(MIPMAPPING_TILE_RES_HALF, MIPMAPPING_TILE_RES_HALF, MIPMAPPING_TILE_RES_HALF)]
void MipmapAnisotropicVoxelMapCS(uint3 globalIdx : SV_DispatchThreadID, 
								 uint3 localIdx	 : SV_GroupThreadID,
								 uint3 groupIdx	 : SV_GroupID)
{
	uint3 lowerMipIdx = globalIdx * 2; // or source

	uint3 sampleIdx[8] =
	{
		( lowerMipIdx + uint3(0, 0, 0) ),
		( lowerMipIdx + uint3(1, 0, 0) ),
		( lowerMipIdx + uint3(0, 1, 0) ),
		( lowerMipIdx + uint3(1, 1, 0) ),
		( lowerMipIdx + uint3(0, 0, 1) ),
		( lowerMipIdx + uint3(1, 0, 1) ),
		( lowerMipIdx + uint3(0, 1, 1) ),
		( lowerMipIdx + uint3(1, 1, 1) ) 
	};

	uint negx = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[0], 0), GetColorFromVoxelMap(sampleIdx[2], 0), GetColorFromVoxelMap(sampleIdx[4], 0), GetColorFromVoxelMap(sampleIdx[6], 0),	//Fronts
		GetColorFromVoxelMap(sampleIdx[1], 0), GetColorFromVoxelMap(sampleIdx[3], 0), GetColorFromVoxelMap(sampleIdx[5], 0), GetColorFromVoxelMap(sampleIdx[7], 0)	//Backs
	);

	uint posx = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[1], 1), GetColorFromVoxelMap(sampleIdx[3], 1), GetColorFromVoxelMap(sampleIdx[5], 1), GetColorFromVoxelMap(sampleIdx[7], 1),	//Fronts
		GetColorFromVoxelMap(sampleIdx[0], 1), GetColorFromVoxelMap(sampleIdx[2], 1), GetColorFromVoxelMap(sampleIdx[4], 1), GetColorFromVoxelMap(sampleIdx[6], 1)	//Backs
	);

	uint negy = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[0], 2), GetColorFromVoxelMap(sampleIdx[1], 2), GetColorFromVoxelMap(sampleIdx[4], 2), GetColorFromVoxelMap(sampleIdx[5], 2),	//Fronts
		GetColorFromVoxelMap(sampleIdx[2], 2), GetColorFromVoxelMap(sampleIdx[3], 2), GetColorFromVoxelMap(sampleIdx[6], 2), GetColorFromVoxelMap(sampleIdx[7], 2)	//Backs
	);
	
	uint posy = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[2], 3), GetColorFromVoxelMap(sampleIdx[3], 3), GetColorFromVoxelMap(sampleIdx[6], 3), GetColorFromVoxelMap(sampleIdx[7], 3),	//Fronts
		GetColorFromVoxelMap(sampleIdx[0], 3), GetColorFromVoxelMap(sampleIdx[1], 3), GetColorFromVoxelMap(sampleIdx[4], 3), GetColorFromVoxelMap(sampleIdx[5], 3)	//Backs
	);

	uint negz = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[0], 4), GetColorFromVoxelMap(sampleIdx[1], 4), GetColorFromVoxelMap(sampleIdx[2], 4), GetColorFromVoxelMap(sampleIdx[3], 4),	//Fronts
		GetColorFromVoxelMap(sampleIdx[4], 4), GetColorFromVoxelMap(sampleIdx[5], 4), GetColorFromVoxelMap(sampleIdx[6], 4), GetColorFromVoxelMap(sampleIdx[7], 4)	//Backs
	);
	
	uint posz = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[4], 5), GetColorFromVoxelMap(sampleIdx[5], 5), GetColorFromVoxelMap(sampleIdx[6], 5), GetColorFromVoxelMap(sampleIdx[7], 5),	//Fronts
		GetColorFromVoxelMap(sampleIdx[0], 5), GetColorFromVoxelMap(sampleIdx[1], 5), GetColorFromVoxelMap(sampleIdx[2], 5), GetColorFromVoxelMap(sampleIdx[3], 5)	//Backs
	);


	uint destDimension = mipmapInfo_sourceDimension / 2;

	OutMipMap[uint3(globalIdx.x,						globalIdx.yz)]	= negx;
	OutMipMap[uint3(globalIdx.x + (1 * destDimension),	globalIdx.yz)]	= posx;
	OutMipMap[uint3(globalIdx.x + (2 * destDimension),	globalIdx.yz)]	= negy;
	OutMipMap[uint3(globalIdx.x + (3 * destDimension),	globalIdx.yz)]	= posy;
	OutMipMap[uint3(globalIdx.x + (4 * destDimension),	globalIdx.yz)]	= negz;
	OutMipMap[uint3(globalIdx.x + (5 * destDimension),	globalIdx.yz)]	= posz;
}