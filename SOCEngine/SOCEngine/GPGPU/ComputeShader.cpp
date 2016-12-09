#include "ComputeShader.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;
using namespace Rendering::Shader;
using namespace Rendering;

ComputeShader::ComputeShader(const ThreadGroup& threadGroup, ID3DBlob* blob)
	: ShaderForm(blob, "CS"), _shader(nullptr), _threadGroup(threadGroup)
{

}

ComputeShader::~ComputeShader(void)
{
	SAFE_RELEASE(_shader);
}

bool ComputeShader::Initialize()
{
	if(_blob == nullptr)
		return false;

	const DirectX* dx = Director::SharedInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	HRESULT hr = device->CreateComputeShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), 0, &_shader);
	ASSERT_MSG_IF(SUCCEEDED(hr), "Error!, system does not create compute shaer component");

	return true;
}

void ComputeShader::Dispatch(ID3D11DeviceContext* context)
{
	context->CSSetShader(_shader, nullptr, 0);
	for(auto iter = _inputSRBuffers.begin(); iter != _inputSRBuffers.end(); ++iter)
	{
		auto buffer = iter->srBuffer;
		if(buffer)
		{
			auto srv = buffer->GetShaderResourceView()->GetView();
			context->CSSetShaderResources(iter->bindIndex, 1, &srv);
		}
	}
	for(auto iter = _inputConstBuffers.begin(); iter != _inputConstBuffers.end(); ++iter)
	{
		auto cb = iter->buffer;
		if(cb)
		{
			auto buffer = cb->GetBuffer();
			context->CSSetConstantBuffers(iter->bindIndex, 1, &buffer);
		}
	}
	for(auto iter = _inputTextures.begin(); iter != _inputTextures.end(); ++iter)
	{
		auto texture = iter->texture;
		if(texture)
		{
			auto srv = texture->GetShaderResourceView()->GetView();
			context->CSSetShaderResources(iter->bindIndex, 1, &srv);
		}
	}

	for(auto iter = _uavs.begin(); iter != _uavs.end(); ++iter)
	{
		if(iter->uav)
		{
			ID3D11UnorderedAccessView* uav = iter->uav->GetView();
			context->CSSetUnorderedAccessViews(iter->bindIndex, 1, &uav, nullptr);
		}
	}

	context->Dispatch(_threadGroup.x, _threadGroup.y, _threadGroup.z);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	{
		for(auto iter = _inputSRBuffers.begin(); iter != _inputSRBuffers.end(); ++iter)
			context->CSSetShaderResources(iter->bindIndex, 1, &nullSRV);

		for(auto iter = _inputTextures.begin(); iter != _inputTextures.end(); ++iter)
			context->CSSetShaderResources(iter->bindIndex, 1, &nullSRV);
	}

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	{
		for(auto iter = _uavs.begin(); iter != _uavs.end(); ++iter)
			context->CSSetUnorderedAccessViews(iter->bindIndex, 1, &nullUAV, nullptr);
	}
	context->CSSetShader(nullptr, nullptr, 0);
}

void ComputeShader::BindTexture(ID3D11DeviceContext* context, TextureBindIndex bind, const Texture::TextureForm* tex)
{
	ID3D11ShaderResourceView* srv = tex ? tex->GetShaderResourceView()->GetView() : nullptr;
	context->CSSetShaderResources(uint(bind), 1, &srv);
}

void ComputeShader::BindSamplerState(ID3D11DeviceContext* context, SamplerStateBindIndex bind, ID3D11SamplerState* samplerState)
{
	context->CSSetSamplers(uint(bind), 1, &samplerState);
}

void ComputeShader::BindConstBuffer(ID3D11DeviceContext* context, ConstBufferBindIndex bind, const Buffer::ConstBuffer* cb)
{
	ID3D11Buffer* buf = cb ? cb->GetBuffer() : nullptr;
	context->CSSetConstantBuffers(uint(bind), 1, &buf);
}

void ComputeShader::BindShaderResourceBuffer(ID3D11DeviceContext* context, TextureBindIndex bind, const Buffer::ShaderResourceBuffer* srBuffer)
{
	ID3D11ShaderResourceView* srv = srBuffer ? srBuffer->GetShaderResourceView()->GetView() : nullptr;
	context->CSSetShaderResources(uint(bind), 1, &srv);
}

void ComputeShader::BindUnorderedAccessView(ID3D11DeviceContext* context, UAVBindIndex bind, const View::UnorderedAccessView* uav, const uint* initialCounts)
{
	ID3D11UnorderedAccessView* view = uav ? uav->GetView() : nullptr;
	context->CSSetUnorderedAccessViews(uint(bind), 1, &view, initialCounts);
}
