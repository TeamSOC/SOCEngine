#pragma once

#include "Texture3D.h"
#include <vector>

namespace Rendering
{
	namespace GI
	{
		class VoxelMap final
		{
		public:
			enum class Direction : uint32
			{
				PosX, NegX,
				PosY, NegY,
				PosZ, NegZ,
				Num
			};

		public:
			void Initialize(Device::DirectX& dx, uint32 sideLength, DXGI_FORMAT tex3DFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint32 mipmapCount, bool isAnisotropic);
			void Destroy();

			GET_CONST_ACCESSOR(SideLength,		uint32,		_sideLength);
			GET_CONST_ACCESSOR(MaxMipmapLevel,	uint32,		_mipmapCount-1);
			GET_CONST_ACCESSOR(MipmapCount,		uint32,		_mipmapCount);

			GET_CONST_ACCESSOR_REF(Texture3D,				_tex3D);

			GET_CONST_ACCESSOR(SourceMapUAV, const	View::UnorderedAccessView*, _tex3D.GetUnorderedAccessView());	
			GET_ACCESSOR(SourceMapUAV,				View::UnorderedAccessView*, _tex3D.GetUnorderedAccessView());	

			auto& GetMipmapUAV(uint32 index)				{ return _mipmapUAVs[index]; }
			const auto& GetMipmapUAV(uint32 index) const	{ return _mipmapUAVs[index]; }

		private:
			Texture::Texture3D						_tex3D;
			std::vector<View::UnorderedAccessView>	_mipmapUAVs;
			uint32									_sideLength;
			uint32									_mipmapCount;
		};
	}
}