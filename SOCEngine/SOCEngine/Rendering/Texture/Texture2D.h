#pragma once

#include "Color.h"
#include "Rect.h"

#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Manager
	{
		class Texture2DManager;
	}

	namespace Texture
	{
		class Texture2D final
		{
		public:
			Texture2D() = default;
			explicit Texture2D(const View::ShaderResourceView& srv, const DXSharedResource<ID3D11Texture2D>& tex, bool hasAlpha, const Size<uint32>& size = Size<uint32>(0, 0));
			explicit Texture2D(const DXSharedResource<ID3D11Texture2D>& tex, const Size<uint32>& size);

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, uint32 width, uint32 height, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint32 bindFlags, uint32 sampleCount, uint32 mipLevels);
			void Initialize(Device::DirectX& dx, const D3D11_TEXTURE2D_DESC& texDesc, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat);
			void Destroy();

			void GenerateMips(Device::DirectX& dx);

			const Size<uint32>& FetchSize();

			GET_CONST_ACCESSOR(Texture,				DXSharedResource<ID3D11Texture2D>,			_texture);
			GET_ACCESSOR(RawTexture,				ID3D11Texture2D* const,						_texture.GetRaw());

			GET_CONST_ACCESSOR(Size,				const Size<uint32>&,							_size);
			GET_CONST_ACCESSOR(CanUse,				bool,										_texture.IsCanUse());

			SET_ACCESSOR(Size,						const Size<uint32>&,							_size);
			SET_ACCESSOR(Texture,					const DXSharedResource<ID3D11Texture2D>&,	_texture);

			GET_CONST_ACCESSOR_REF(ShaderResourceView,											_srv);
			GET_CONST_ACCESSOR_REF(UnorderedAccessView,											_uav);

		private:
			DXSharedResource<ID3D11Texture2D>		_texture;
			Size<uint32>								_size = Size<uint32>(0, 0);
			View::ShaderResourceView				_srv;
			View::UnorderedAccessView				_uav;
		};
	}
}
