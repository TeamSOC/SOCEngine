#include "GeometryShader.h"

using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering;

GeometryShader::GeometryShader(const DXResource<ID3DBlob>& blob, const std::string& key)
	: _shader(nullptr), _baseShader(blob, key)
{
}

void GeometryShader::Initialize(Device::DirectX& dx)
{
	_shader = dx.CreateGeometryShader(_baseShader);
}

void GeometryShader::BindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->GSSetShader(_shader.GetRaw(), nullptr, 0);
}

void GeometryShader::UnBindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->GSSetShader(nullptr, nullptr, 0);
}

void GeometryShader::BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, const View::ShaderResourceView& srv)
{
	ID3D11ShaderResourceView* raw = srv.GetView().GetRaw();
	dx.GetContext()->GSSetShaderResources(static_cast<uint>(bind), 1, &raw);
}

void GeometryShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, DXResource<ID3D11SamplerState>& samplerState)
{	
	ID3D11SamplerState* sampler = samplerState.GetRaw();
	dx.GetContext()->GSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void GeometryShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb)
{
	ID3D11Buffer* buf = cb.GetBaseBuffer().GetBuffer().GetRaw();
	dx.GetContext()->GSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void GeometryShader::UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->GSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void GeometryShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->GSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void GeometryShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->GSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}
