#include "VoxelMap.h"

using namespace Device;
using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::View;

void VoxelMap::Initialize(
	DirectX& dx,
	uint32 sideLength, DXGI_FORMAT tex3DFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint32 mipmapCount, bool isAnisotropic)
{
	_sideLength		= sideLength;
	_mipmapCount	= mipmapCount;

	uint32 width		= sideLength * (isAnisotropic ? (uint32)Direction::Num : 1);
	uint32 height		= sideLength;
	uint32 depth		= sideLength;

	_tex3D.Initialize(dx, width, height, depth, tex3DFormat, srvFormat, uavFormat, D3D11_BIND_RENDER_TARGET, _mipmapCount);

	for(uint32 i=1; i<mipmapCount; ++i)
	{
		using DeviceTexture = std::decay_t<decltype(_tex3D.GetTexture())>;
		DeviceTexture& texture = const_cast<DeviceTexture&>(_tex3D.GetTexture());

		UnorderedAccessView uav;
		uav.Initialize(dx, uavFormat, 0, texture, D3D11_UAV_DIMENSION_TEXTURE3D, i, -1);

		_mipmapUAVs.push_back(uav);
	}
}

void VoxelMap::Destroy()
{
	for (auto& uav : _mipmapUAVs)
		uav.Destroy();

	_tex3D.Destroy();
}
