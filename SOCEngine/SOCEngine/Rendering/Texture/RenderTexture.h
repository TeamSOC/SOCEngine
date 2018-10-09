#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class RenderTexture final
		{
		public:
			RenderTexture() = default;
			explicit RenderTexture(const DXSharedResource<ID3D11RenderTargetView>& rawRtv, const DXSharedResource<ID3D11Texture2D>& rawTex2D, const Size<uint32>& size);

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, const Size<uint32>& size, DXGI_FORMAT srvFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT uavFormat, uint32 optionalBindFlags, uint32 sampleCount = 0, uint32 mipLevel = 1);
			void Initialize(Device::DirectX& dx, const DXSharedResource<ID3D11RenderTargetView>& rtv, const Size<uint32>& size);
			void Destroy();

			void Clear(Device::DirectX& dx, const Color& color);

			//GET_CONST_ACCESSOR(RenderTargetView, const DXSharedResource<ID3D11RenderTargetView>&, _renderTargetView);
			GET_ACCESSOR(Raw,			ID3D11RenderTargetView* const,	_renderTargetView.GetRaw());
			GET_CONST_ACCESSOR(Size,	const auto&,					_tex2D.GetSize());
			GET_ALL_ACCESSOR_PTR(Texture2D,	Texture2D,					_tex2D);

		private:
			DXSharedResource<ID3D11RenderTargetView>	_renderTargetView;
			Texture2D									_tex2D;
		};
	}
}
