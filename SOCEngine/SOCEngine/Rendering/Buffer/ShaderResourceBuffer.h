#pragma once

#include "BaseBuffer.h"
#include "ShaderResourceView.h"

namespace Rendering
{
	namespace Buffer
	{
		class ShaderResourceBuffer
		{
		public:
			ShaderResourceBuffer() = default;
			ShaderResourceBuffer(const BaseBuffer&, const View::ShaderResourceView&);

			void Initialize(Device::DirectX& dx, uint32 stride, uint32 num, DXGI_FORMAT format, const void* sysMem, uint32 optionalBindFlag, D3D11_USAGE usage);
			void Destroy();

			inline void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint32 size)
			{
				_baseBuffer.UpdateResourceUsingMapUnMap(dx, data, size);
			}
			inline void UpdateResourceUsingMapUnMap(Device::DirectX& dx, const void* data, uint32 startOffset, uint32 size, D3D11_MAP mapType)
			{
				_baseBuffer.UpdateResourceUsingMapUnMap(dx, data, startOffset, size, mapType);
			}

			GET_CONST_ACCESSOR_REF(ShaderResourceView,	_srv);
			GET_CONST_ACCESSOR_REF(BaseBuffer,			_baseBuffer);

		private:
			BaseBuffer					_baseBuffer;
			View::ShaderResourceView	_srv;
		};
	}

}
