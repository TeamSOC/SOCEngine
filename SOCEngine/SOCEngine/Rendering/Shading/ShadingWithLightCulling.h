#pragma once

#include "LightCulling.h"
#include "RenderTexture.h"

namespace Rendering
{
	namespace TBDR
	{
		class ShadingWithLightCulling : public Light::LightCulling
		{
		public:
			enum class OutputBufferShaderIndex : unsigned int
			{
				OutScreen = 0
			};
			struct GBuffers
			{
				const Texture::RenderTexture* albedo_emission;
				const Texture::RenderTexture* specular_metallic;
				const Texture::RenderTexture* normal_roughness;
			};

		private:
			Rendering::Texture::RenderTexture*					_offScreen;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputPointLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputSpotLightColorBuffer;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightParamBuffer;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightShadowParamBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputPointLightShadowParamBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputSpotLightShadowParamBuffer;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightShadowColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputPointLightShadowColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputSpotLightShadowColorBuffer;

		public:
			ShadingWithLightCulling();
			virtual ~ShadingWithLightCulling();

		public:
			void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const GBuffers& geometryBuffers, const Math::Size<uint>& backBufferSize, bool useDebugMode = false);
			void Destory();

			void Dispatch(const Device::DirectX* dx, const Buffer::ConstBuffer* tbrConstBuffer, const Buffer::ConstBuffer* shadowGlobalParamConstBuffer);

		public:
			GET_ACCESSOR(OffScreen, const Rendering::Texture::RenderTexture*, _offScreen);
		};
	}
}