#pragma once

#include <d3d11_1.h>
#include "DXResource.h"

namespace Device
{
	class DirectX;
}

namespace Rendering
{
	namespace Buffer
	{
		class BaseBuffer
		{
		public:
			using Key = uint;

		public:
			BaseBuffer() = default;
			explicit BaseBuffer(const DXSharedResource<ID3D11Buffer>& buffer) : _buffer(buffer) {}

			void Destroy();

			SET_ACCESSOR(Buffer,		const DXSharedResource<ID3D11Buffer>&,		_buffer);
			GET_CONST_ACCESSOR(Buffer,	DXSharedResource<ID3D11Buffer>,				_buffer);

			GET_ACCESSOR(Raw,			ID3D11Buffer* const,						_buffer.GetRaw());

			void UpdateSubResource(Device::DirectX& dx, const void* data);
			void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint size);
			void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint startOffset, uint size, D3D11_MAP mapType);

		private:
			DXSharedResource<ID3D11Buffer> _buffer;
		};
	}
}
