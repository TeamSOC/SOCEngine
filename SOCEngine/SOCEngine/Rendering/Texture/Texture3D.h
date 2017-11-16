#pragma once

#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture3D final
		{
		public:
			Texture3D() = default;

			void Initialize(Device::DirectX& dx,
							uint width, uint height, uint depth,
							DXGI_FORMAT tex3DFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
							uint optionBindFlag, uint mipLevels);

			GET_ACCESSOR_REF(Texture,								_texture);
			GET_ACCESSOR_REF(RenderTargetView,						_rtv);
			GET_ACCESSOR_REF(ShaderResourceView,					_srv);
			GET_ACCESSOR_REF(UnorderedAccessView,					_uav);

			GET_CONST_ACCESSOR(Size,				const auto&,	_size);

		private:
			Math::Vector3								_size = Math::Vector3(0.0f, 0.0f, 0.0f);

			DXSharedResource<ID3D11Texture3D>			_texture;
			DXSharedResource<ID3D11RenderTargetView>	_rtv;
			View::ShaderResourceView					_srv;
			View::UnorderedAccessView					_uav;
		};
	}
}
