#include "UnorderedAccessView.h"
#include "DirectX.h"

using namespace Rendering::View;

void UnorderedAccessView::Destroy()
{
	_uav.Destroy();
}

void UnorderedAccessView::Initialize(
	Device::DirectX& dx,
	DXGI_FORMAT format, uint32 numElements, ID3D11Resource* resource,
	D3D11_UAV_DIMENSION viewDimension,
	uint32 tex3dMipSlice, uint32 tex3dWSize,
	uint32 bufferFlags)
{
	assert(_uav.GetRaw() == nullptr);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	desc.Format					= format;
	desc.ViewDimension			= viewDimension;
	desc.Buffer.FirstElement	= 0;
	desc.Buffer.NumElements		= numElements;
	desc.Buffer.Flags			= bufferFlags;

	if(viewDimension == D3D11_UAV_DIMENSION_TEXTURE3D)
	{
		desc.Texture3D.FirstWSlice	= 0;
		desc.Texture3D.MipSlice		= tex3dMipSlice;
		desc.Texture3D.WSize		= tex3dWSize;
	}

	_uav = dx.CreateUnorderedAccessView(resource, desc);
}
