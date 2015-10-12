#include "DirectX.h"
#include "Utility.h"

using namespace Device;

DirectX::DirectX(void) :
	_device(nullptr), _swapChain(nullptr), _immediateContext(nullptr),
	_renderTargetView(nullptr), _rasterizerClockwiseDisableCulling(nullptr),
	_opaqueBlend(nullptr), _alphaToCoverageBlend(nullptr),
	_depthDisableDepthTest(nullptr), _depthLess(nullptr), 
	_depthEqualAndDisableDepthWrite(nullptr), _depthGreater(nullptr),
	_depthGreaterAndDisableDepthWrite(nullptr), _alphaBlend(nullptr),
	_anisotropicSamplerState(nullptr), _linearSamplerState(nullptr), _pointSamplerState(nullptr),
	_rasterizerClockwiseDefault(nullptr), _rasterizerCounterClockwiseDisableCulling(nullptr),
	_rasterizerCounterClockwiseDefault(nullptr)
{
	memset(&_msaaDesc, 0, sizeof(DXGI_SAMPLE_DESC));
}

DirectX::~DirectX(void)
{
}

bool DirectX::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;

	HRESULT hr = _swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer );

	if( FAILED(hr) )
		return false;

	hr = _device->CreateRenderTargetView( backBuffer, nullptr, &_renderTargetView );
	backBuffer->Release();

	if( FAILED(hr) )
		return false;

	return true;
}

bool DirectX::CreateDeviceAndSwapChain(const Win32* win, const DXGI_SAMPLE_DESC* multiSampler)
{
	//swapChain setting
	DXGI_SWAP_CHAIN_DESC	sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

	const Math::Size<unsigned int>& winSize = win->GetSize();
	sd.BufferDesc.Width = winSize.w;
	sd.BufferDesc.Height = winSize.h;

	sd.BufferCount = 1;
	//sd.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = win->GetHandle();
	sd.Windowed = win->GetIsWindowMode();
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//msaa
	if(multiSampler == nullptr)
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	else
	{
		sd.SampleDesc = (*multiSampler);
	}

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	int numFeatureLevels = ARRAYSIZE( featureLevels );

	unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = E_FAIL;
	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, _driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &_featureLevel, &_immediateContext );

		if( SUCCEEDED( hr ) )
            break;
    }

	_msaaDesc = sd.SampleDesc;

    if( FAILED( hr ) )
        return false;

	return true;
}

bool DirectX::InitViewport(const Math::Rect<uint>& rect)
{
	D3D11_VIEWPORT vp;

	vp.Width = (float)rect.size.w;
	vp.Height = (float)rect.size.h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = (float)rect.x;
	vp.TopLeftY = (float)rect.y;

	_immediateContext->RSSetViewports( 1, &vp );

	_backBufferSize.w = (unsigned int)vp.Width;
	_backBufferSize.h = (unsigned int)vp.Height;

	return true;
}

void DirectX::CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format)
{
	unsigned int ableMultiSample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	for(ableMultiSample; ableMultiSample >= 0; ableMultiSample/=2)
	{
		DXGI_SAMPLE_DESC desc;
		HRESULT hr = _device->CheckMultisampleQualityLevels( format, ableMultiSample, &desc.Quality);

		if( FAILED(hr) )
			break;

		desc.Count = ableMultiSample;
		desc.Quality -= 1;

		outDescs.push_back(desc);
	}
}

void DirectX::CreateBlendStates()
{
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable				= false;
	blendDesc.IndependentBlendEnable			= false;

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;

	renderTargetBlendDesc.BlendEnable			= false;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_ONE; 
	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_ZERO; 

	renderTargetBlendDesc.BlendOp				= D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_ONE; 

	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_ZERO; 
	renderTargetBlendDesc.BlendOpAlpha			= D3D11_BLEND_OP_ADD;

	renderTargetBlendDesc.RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	blendDesc.RenderTarget[1] = renderTargetBlendDesc;
	blendDesc.RenderTarget[2] = renderTargetBlendDesc;

	if( FAILED(_device->CreateBlendState(&blendDesc, &_opaqueBlend)) )
		ASSERT_MSG("Error!, device cant create opaque blend state");

	blendDesc.AlphaToCoverageEnable				= true;
	if( FAILED(_device->CreateBlendState(&blendDesc, &_alphaToCoverageBlend)) )
		ASSERT_MSG("Error!, device cant create alphaToCoverage blend state");

	blendDesc.AlphaToCoverageEnable				= false;
	renderTargetBlendDesc.BlendEnable			= true;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;

	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	blendDesc.RenderTarget[1] = renderTargetBlendDesc;
	blendDesc.RenderTarget[2] = renderTargetBlendDesc;

	if( FAILED(_device->CreateBlendState(&blendDesc, &_alphaBlend)) )
		ASSERT_MSG("Error!, device cant create _alphaBlend blend state");
}

bool DirectX::InitDevice(const Win32* win, const Math::Rect<uint>& renderScreenRect)
{
	if( CreateDeviceAndSwapChain(win) == false )
		return false;

	if( CreateRenderTargetView() == false )
		return false;

	_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	InitViewport(renderScreenRect);

	//Initialize rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode				= D3D11_FILL_SOLID;
		desc.CullMode				= D3D11_CULL_NONE;		//�ø� ��
		desc.FrontCounterClockwise	= true;
		desc.DepthBias				= 0;
		desc.DepthBiasClamp			= 0.0f;
		desc.SlopeScaledDepthBias	= 0.0f;
		desc.DepthClipEnable		= true;
		desc.ScissorEnable			= false;
		desc.MultisampleEnable		= false;
		desc.AntialiasedLineEnable	= false;

		if( FAILED(_device->CreateRasterizerState(&desc, &_rasterizerCounterClockwiseDisableCulling)) )
			ASSERT_MSG("Error!, device cant create rasterizer state");

		desc.FrontCounterClockwise	= false;
		if( FAILED(_device->CreateRasterizerState(&desc, &_rasterizerClockwiseDisableCulling)) )
			ASSERT_MSG("Error!, device cant create rasterizer state");

		desc.FrontCounterClockwise	= true;
		desc.CullMode = D3D11_CULL_BACK;
		if( FAILED(_device->CreateRasterizerState(&desc, &_rasterizerCounterClockwiseDefault)) )
			ASSERT_MSG("Error!, device cant create rasterizer state");

		desc.FrontCounterClockwise	= false;
		if( FAILED(_device->CreateRasterizerState(&desc, &_rasterizerClockwiseDefault)) )
			ASSERT_MSG("Error!, device cant create rasterizer state");
	}
	
	//Initialize Blend State
	CreateBlendStates();

	//Initialize Depth State
	//using inverted 32bit float depth
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable		= true; 
		desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL; 
		desc.DepthFunc			= D3D11_COMPARISON_GREATER; //inverted depth
		desc.StencilEnable		= false; 
		desc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK; 
		desc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK; 
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthGreater)) )
			ASSERT_MSG("Error!, device cant create lessEqual dpeth state");

		//disable depth write
		desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ZERO; 
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthDisableDepthWrite)) )
			ASSERT_MSG("Error!, device cant create _depthDisableDepthWrite");
		
		//disable depth test
		desc.DepthEnable = false;
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthDisableDepthTest)) )
			ASSERT_MSG("Error!, device cant create _depthDisableDepthTest");

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_GREATER; //inverted
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthGreaterAndDisableDepthWrite)) )
			ASSERT_MSG("Error!, device cant create _depthGreaterAndDisableDepthWrite");

		desc.DepthFunc = D3D11_COMPARISON_EQUAL;
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthEqualAndDisableDepthWrite)) )
			ASSERT_MSG("Error!, device cant create _depthEqualAndDisableDepthWrite");

		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		if( FAILED(_device->CreateDepthStencilState( &desc, &_depthLess)) )
			ASSERT_MSG("Error!, device cant create _depthEqualAndDisableDepthWrite");
	}

	//sampler
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof(sampDesc) );
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = _device ->CreateSamplerState( &sampDesc, &_anisotropicSamplerState );
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, device cant create sampler state");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = _device ->CreateSamplerState( &sampDesc, &_linearSamplerState );
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, device cant create sampler state");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = _device ->CreateSamplerState( &sampDesc, &_pointSamplerState );
		ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, device cant create sampler state");
	}

	return true;
}

unsigned int DirectX::CalcFormatSize(DXGI_FORMAT format) const
{
	if(		DXGI_FORMAT_R32G32B32A32_TYPELESS	<= format && format <= DXGI_FORMAT_R32G32B32A32_SINT)
		return 16;
	else if(DXGI_FORMAT_R32G32B32_TYPELESS		<= format && format <= DXGI_FORMAT_R32G32B32_SINT)
		return 12;
	else if(DXGI_FORMAT_R16G16B16A16_TYPELESS	<= format && format <= DXGI_FORMAT_R16G16B16A16_SINT)
		return 8;
	else if(DXGI_FORMAT_R32G32_TYPELESS			<= format && format <= DXGI_FORMAT_R32G32_SINT)
		return 8;
	else if(DXGI_FORMAT_R8G8B8A8_TYPELESS		<= format && format <= DXGI_FORMAT_R8G8B8A8_SINT)
		return 4;
	else if(DXGI_FORMAT_R16G16_TYPELESS			<= format && format <= DXGI_FORMAT_R16G16_SINT)
		return 4;
	else if(DXGI_FORMAT_R32_TYPELESS			<= format && format <= DXGI_FORMAT_R32_SINT)
		return 4;
	else if(DXGI_FORMAT_R8G8_TYPELESS			<= format && format <= DXGI_FORMAT_R8G8_SINT)
		return 4;
	else if(DXGI_FORMAT_R16_TYPELESS			<= format && format <= DXGI_FORMAT_R16_SINT)
		return 2;
	else if(DXGI_FORMAT_R8_TYPELESS				<= format && format <= DXGI_FORMAT_R8_SINT)
		return 1;

	DEBUG_LOG("Not Support Format");
	return 0;
}

void DirectX::ClearDeviceContext() const
{
    ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11RenderTargetView* pRTVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11DepthStencilView* pDSV = NULL;
    ID3D11Buffer* pBuffers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11SamplerState* pSamplers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    // Constant buffers
    _immediateContext->VSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->PSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->CSSetConstantBuffers( 0, 14, pBuffers );

    // Resources
    _immediateContext->VSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->PSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->CSSetShaderResources( 0, 16, pSRVs );

    // Samplers
    _immediateContext->VSSetSamplers( 0, 16, pSamplers );
    _immediateContext->PSSetSamplers( 0, 16, pSamplers );
    _immediateContext->CSSetSamplers( 0, 16, pSamplers );

    // Render targets
    _immediateContext->OMSetRenderTargets( 8, pRTVs, pDSV );

    // States
    FLOAT BlendFactor[4] = { 0,0,0,0 };
    _immediateContext->OMSetBlendState( NULL, BlendFactor, 0xFFFFFFFF );
	_immediateContext->OMSetDepthStencilState( _depthGreater, 0x00 );  // we are using inverted 32-bit float depth for better precision
	_immediateContext->RSSetState(nullptr);
}

Rendering::Shader::ShaderMacro DirectX::GetMSAAShaderMacro() const
{
	Rendering::Shader::ShaderMacro shaderMacro;

	shaderMacro.SetName("MSAA_SAMPLES_COUNT");
	shaderMacro.SetDefinition( std::to_string(_msaaDesc.Count) );

	return shaderMacro;
}

Math::Size<uint> DirectX::FetchBackBufferSize()
{
	uint num = 1;
	D3D11_VIEWPORT vp;
	_immediateContext->RSGetViewports(&num, &vp);

	_backBufferSize.w = (unsigned int)vp.Width;
	_backBufferSize.h = (unsigned int)vp.Height;

	return _backBufferSize;
}

void DirectX::GetViewportMatrix(Math::Matrix& outMat) const
{
	uint num = 1;
	D3D11_VIEWPORT vp;
	_immediateContext->RSGetViewports(&num, &vp);

	outMat._11 = vp.Width /  2.0f;
	outMat._12 = 0.0f;
	outMat._13 = 0.0f;
	outMat._14 = 0.0f;

	outMat._21 = 0.0f;
	outMat._22 = -vp.Height / 2.0f;
	outMat._23 = 0.0f;
	outMat._24 = 0.0f;

	outMat._31 = 0.0f;
	outMat._32 = 0.0f;
	outMat._33 = vp.MaxDepth - vp.MinDepth;
	outMat._34 = 0.0f;

	outMat._41 = vp.TopLeftX + vp.Width / 2.0f;
	outMat._42 = vp.TopLeftY + vp.Height / 2.0f;
	outMat._43 = vp.MinDepth;
	outMat._44 = 1.0f;
}