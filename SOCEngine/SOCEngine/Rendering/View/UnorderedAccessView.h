#pragma once

#include <d3d11_1.h>
#include "DXResource.h"
namespace Device
{
	class DirectX;
}

namespace Rendering
{
	namespace View
	{
		class UnorderedAccessView final
		{
		public:
			UnorderedAccessView() = default;
			UnorderedAccessView(const DXSharedResource<ID3D11UnorderedAccessView>& uav) : _uav(uav) {}

			GET_CONST_ACCESSOR(View,	DXSharedResource<ID3D11UnorderedAccessView>,	_uav);
			GET_ACCESSOR(Raw,			ID3D11UnorderedAccessView* const,				_uav.GetRaw());

			template <class DXResourceType>
			void Initialize(Device::DirectX& dx, DXGI_FORMAT format, uint32 numElements, DXResourceType& resource, D3D11_UAV_DIMENSION viewDimension, uint32 tex3dMipSlice = 0, uint32 tex3dWSize = 0, uint32 bufferFlags = 0)
			{				
				Initialize(dx, format, numElements, resource.GetRaw(), viewDimension, tex3dMipSlice, tex3dWSize, bufferFlags);
			}
			void Destroy();

		private:
			void Initialize(Device::DirectX& dx, DXGI_FORMAT format, uint32 numElements, ID3D11Resource* resource, D3D11_UAV_DIMENSION viewDimension, uint32 tex3dMipSlice = 0, uint32 tex3dWSize = 0, uint32 bufferFlags = 0);

		private:
			DXSharedResource<ID3D11UnorderedAccessView>		_uav;
		};
	}
}
