#pragma once

#include "GPUUploadBuffer.hpp"
#include "CommonLightBuffer.h"
#include "Transform.h"
#include <assert.h>

namespace Rendering
{
	namespace Light
	{
		namespace Buffer
		{
			struct RequiredIndexer
			{
				const Core::ObjectID::IndexHashMap& shadowIndexer;
				const Core::ObjectID::IndexHashMap& lightShaftIndexer;
				RequiredIndexer(
					const Core::ObjectID::IndexHashMap& _shadowIndexer,
					const Core::ObjectID::IndexHashMap& _lightShaftIndexer)
					: shadowIndexer(_shadowIndexer), lightShaftIndexer(_lightShaftIndexer)
				{

				}
			};

			template<typename LightType>
			class LightBufferForm
			{
			public:
				using TransformType				= typename LightType::TransformType;
				using TransformBuffer			= Rendering::Buffer::GPUUploadBuffer<TransformType>;
				
			public:
				LightBufferForm() = default;
				
				void Initialize(Device::DirectX& dx, uint maxLightCount, DXGI_FORMAT tfFormat)
				{
					TransformType dummy[POINT_LIGHT_BUFFER_MAX_NUM];
					memset(dummy, 0, sizeof(TransformType) * POINT_LIGHT_BUFFER_MAX_NUM);
					_transformBuffer.Initialize(dx, maxLightCount, tfFormat, dummy);

					_commonBuffer.Initialize(dx, maxLightCount, dummy);

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}

				void PushLight(const LightType& light)
				{
					_transformBuffer.PushData(LightType::TransformType());
					_commonBuffer.PushData(light.GetBase(), -1, -1);

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}

				void DeleteAll()
				{
					_transformBuffer.DeleteAll();
					_commonBuffer.DeleteAll();

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}

			public:
				void UpdateTransformBuffer(	const std::vector<LightType*>& dirtyTFLights,
											const Core::TransformPool& tfPool,
											const Core::ObjectID::IndexHashMap& indexer)
				{
					for (const auto& light : dirtyTFLights)
					{
						Core::ObjectID objID = light->GetObjectID();
						const auto& tf = tfPool.Find(objID.Literal());
						
						uint index = indexer.Find(objID.Literal());
						_transformBuffer[index] = light->MakeTransform(*tf);
					}

					_mustUpdateTransformSRBuffer |= (dirtyTFLights.empty() != false);
				}

				void UpdateLightCommonBuffer(const std::vector<LightType*>& dirtyParamLights, RequiredIndexer indexers, const Core::ObjectID::IndexHashMap& indexer)
				{
					for (auto& light : dirtyParamLights)
					{
						Core::ObjectID objID = light->GetObjectID();
						uint literalID = objID.Literal();

						ushort shadowIdx = indexers.shadowIndexer.Find(literalID);
						uint lightShaftIdx = indexers.lightShaftIndexer.Find(literalID);

						uint index = indexer.Find(literalID);
						_commonBuffer.SetData(index, light->GetBase(), shadowIdx, lightShaftIdx);
					}

					_mustUpdateCommonSRBuffer |= (dirtyParamLights.empty() != false);
				}

				void UpdateSRBuffer(Device::DirectX& dx, bool forcedUpdate)
				{
					if(_mustUpdateTransformSRBuffer | forcedUpdate)		_transformBuffer.UpdateSRBuffer(dx);
					if(_mustUpdateCommonSRBuffer	| forcedUpdate)		_commonBuffer.UpdateSRBuffer(dx);

					_mustUpdateCommonSRBuffer =
					_mustUpdateTransformSRBuffer = false;
				}
				void Delete(uint index)
				{
					_transformBuffer.Delete(index);
					_commonBuffer.Delete(index);

					_mustUpdateTransformSRBuffer = true;
					_mustUpdateCommonSRBuffer = true;
				}
				
				
			public:
				GET_ACCESSOR(TransformSRBuffer,				auto&, _transformBuffer.GetShaderResourceBuffer());
				GET_ACCESSOR(ColorSRBuffer,					auto&, _commonBuffer.GetColorSRBuffer());
				GET_ACCESSOR(OptionalParamIndexSRBuffer, 	auto&, _commonBuffer.GetOptionalParamIndexSRBuffer());

				GET_CONST_ACCESSOR(Size, uint, _transformBuffer.GetSize());

			protected:
				TransformBuffer					_transformBuffer;
				CommonLightBuffer				_commonBuffer;

				bool							_mustUpdateCommonSRBuffer		= true;
				bool							_mustUpdateTransformSRBuffer	= true;
			};
		}
	}
}