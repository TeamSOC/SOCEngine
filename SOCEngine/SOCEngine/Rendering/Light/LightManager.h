#pragma once

#include "Lights.h"
#include "Frustum.h"
#include "VectorHashMap.h"
#include "ShaderResourceBuffer.h"

#define POINT_LIGHT_BUFFER_MAX_NUM			2048
#define SPOT_LIGHT_BUFFER_MAX_NUM			2048
#define DIRECTIONAL_LIGHT_BUFFER_MAX_NUM	1024
#define LIGHT_BUFFER_OVERALL_UPDATE_RATE	0.5f

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		private:
			struct Lights
			{
				const Light::LightForm* light;
				uint prevTransformUpdateCounter;
				Lights(const Light::LightForm* _light, uint updateCounter) 
					: light(_light), prevTransformUpdateCounter(updateCounter) {}
				~Lights(){}
			};
			Structure::VectorHashMap<std::string, Lights>	_lights;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_pointLightTransformBuffer;
			Structure::VectorHashMap<std::string, uint>											_pointLightColorBuffer;
			Structure::VectorHashMap<std::string, uint>											_pointLightShadowColorBuffer;
			Structure::VectorHashMap<std::string, ushort>										_pointLightShadowIndexBuffer;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_directionalLightTransformBuffer;
			Structure::VectorHashMap<std::string, Light::DirectionalLight::Params>				_directionalLightParamBuffer;
			Structure::VectorHashMap<std::string, uint>											_directionalLightColorBuffer;
			Structure::VectorHashMap<std::string, uint>											_directionalLightShadowColorBuffer;
			Structure::VectorHashMap<std::string, ushort>										_directionalLightShadowIndexBuffer;

			Structure::VectorHashMap<std::string, Light::LightForm::LightTransformBuffer>		_spotLightTransformBuffer;
			Structure::VectorHashMap<std::string, Light::SpotLight::Params>						_spotLightParamBuffer;
			Structure::VectorHashMap<std::string, uint>											_spotLightColorBuffer;
			Structure::VectorHashMap<std::string, uint>											_spotLightShadowColorBuffer;
			Structure::VectorHashMap<std::string, ushort>										_spotLightShadowIndexBuffer;

			Buffer::ShaderResourceBuffer*	_pointLightTransformBufferSR;
			Buffer::ShaderResourceBuffer*	_pointLightColorBufferSR;
			Buffer::ShaderResourceBuffer*	_pointLightShadowColorBufferSR;
			Buffer::ShaderResourceBuffer*	_pointLightShadowIndexBufferSR;

			Buffer::ShaderResourceBuffer*	_directionalLightTransformBufferSR;
			Buffer::ShaderResourceBuffer*	_directionalLightParamBufferSR;
			Buffer::ShaderResourceBuffer*	_directionalLightColorBufferSR;
			Buffer::ShaderResourceBuffer*	_directionalLightShadowColorBufferSR;
			Buffer::ShaderResourceBuffer*	_directionalLightShadowIndexBufferSR;

			Buffer::ShaderResourceBuffer*	_spotLightTransformBufferSR;
			Buffer::ShaderResourceBuffer*	_spotLightParamBufferSR;
			Buffer::ShaderResourceBuffer*	_spotLightColorBufferSR;
			Buffer::ShaderResourceBuffer*	_spotLightShadowColorBufferSR;
			Buffer::ShaderResourceBuffer*	_spotLightShadowIndexBufferSR;

			enum class BufferUpdateType : uint
			{
				Overall,
				Selective
			};

			BufferUpdateType _pointLightBufferUpdateType;
			BufferUpdateType _spotLightBufferUpdateType;
			BufferUpdateType _directionalLightBufferUpdateType;

		public:
			LightManager(void);
			~LightManager(void);

		private:
			void UpdateBufferUsingMapDiscard(ID3D11DeviceContext* context);
			void UpdateBufferUsingMapNoOverWrite(ID3D11DeviceContext* context);

		public:
			void InitializeAllShaderResourceBuffer();
			void DestroyAllShaderReourceBuffer();

			void Add(const Light::LightForm* light, const char* key = nullptr);
			void UpdateBuffer(const Device::DirectX* dx);

			bool HasKey(const std::string& key);

			void Delete(const std::string& key);
			void DeleteAll();

			uint GetPackedLightCount() const;

		public:
			GET_ACCESSOR(PointLightTransformBufferSR,			const Buffer::ShaderResourceBuffer*, _pointLightTransformBufferSR);
			GET_ACCESSOR(PointLightColorBufferSR,				const Buffer::ShaderResourceBuffer*, _pointLightColorBufferSR);
			GET_ACCESSOR(PointLightShadowColorBufferSR,			const Buffer::ShaderResourceBuffer*, _pointLightShadowColorBufferSR);
			GET_ACCESSOR(PointLightShadowIndexBufferSR,			const Buffer::ShaderResourceBuffer*, _pointLightShadowIndexBufferSR);

			GET_ACCESSOR(DirectionalLightTransformBufferSR,		const Buffer::ShaderResourceBuffer*, _directionalLightTransformBufferSR);
			GET_ACCESSOR(DirectionalLightParamBufferSR,			const Buffer::ShaderResourceBuffer*, _directionalLightParamBufferSR);
			GET_ACCESSOR(DirectionalLightColorBufferSR,			const Buffer::ShaderResourceBuffer*, _directionalLightColorBufferSR);
			GET_ACCESSOR(DirectionalLightShadowColorBufferSR,	const Buffer::ShaderResourceBuffer*, _directionalLightShadowColorBufferSR);
			GET_ACCESSOR(DirectionalLightShadowIndexBufferSR,	const Buffer::ShaderResourceBuffer*, _directionalLightShadowIndexBufferSR);

			GET_ACCESSOR(SpotLightTransformBufferSR,			const Buffer::ShaderResourceBuffer*, _spotLightTransformBufferSR);
			GET_ACCESSOR(SpotLightParamBufferSR,				const Buffer::ShaderResourceBuffer*, _spotLightParamBufferSR);
			GET_ACCESSOR(SpotLightColorBufferSR,				const Buffer::ShaderResourceBuffer*, _spotLightColorBufferSR);
			GET_ACCESSOR(SpotLightShadowColorBufferSR,			const Buffer::ShaderResourceBuffer*, _spotLightShadowColorBufferSR);
			GET_ACCESSOR(SpotLightShadowIndexBufferSR,			const Buffer::ShaderResourceBuffer*, _spotLightShadowIndexBufferSR);
		};
	}
}