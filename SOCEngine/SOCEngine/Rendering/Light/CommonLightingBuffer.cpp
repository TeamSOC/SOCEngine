#include "CommonLightingBuffer.h"

using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Light::LightingBuffer;

void CommonLightingBuffer::Initialize(Device::DirectX& dx, uint count, const void* dummy)
{
	_colorBuffer.Initialize(dx, count, DXGI_FORMAT_R8G8B8A8_UNORM, dummy);
	_optionalParamIndexBuffer.Initialize(dx, count, DXGI_FORMAT_R32_UINT, dummy);	
}

void CommonLightingBuffer::SetData(const Light::BaseLight& light, ushort shadowIndex, uint shaftIndex)
{	
	uint lightId = light.GetLightId().Literal();

	_colorBuffer.SetData(lightId, light.Get32BitMainColor());
	_optionalParamIndexBuffer.SetData(lightId, ComputeOptionalParamIndex(light, shadowIndex, shaftIndex));
}

void CommonLightingBuffer::AddData(const Light::BaseLight & light, ushort shadowIndex, uint lightShaftIndex)
{
	uint lightId = light.GetLightId().Literal();

	_colorBuffer.AddData(lightId, light.Get32BitMainColor());
	_optionalParamIndexBuffer.AddData(lightId, ComputeOptionalParamIndex(light, shadowIndex, lightShaftIndex));
}

void CommonLightingBuffer::UpdateSRBuffer(Device::DirectX& dx)
{
	_colorBuffer.UpdateSRBuffer(dx);
	_optionalParamIndexBuffer.UpdateSRBuffer(dx);
}

void CommonLightingBuffer::Delete(LightId id)
{
	_colorBuffer.Delete(id.Literal());
	_optionalParamIndexBuffer.Delete(id.Literal());
}

void CommonLightingBuffer::DeleteAll()
{
	_colorBuffer.DeleteAll();
	_optionalParamIndexBuffer.DeleteAll();
}