#pragma once

#include "DirectionalLightShadowMapCBPool.h"
#include "ShadowMapRenderer.h"
#include "ShadowAtlasMap.hpp"

namespace Rendering
{
	namespace Renderer
	{
		namespace ShadowMap
		{
			class DirectionalLightShadowMapRenderer final 
			{
			public:
				using ShadowType = Shadow::DirectionalLightShadow;
				struct Param
				{
					const Manager::MaterialManager&		materialMgr;
					Shadow::ShadowAtlasMap<ShadowType>&	shadowMap;
					uint32								shadowIndex;
					uint32								atlasMapResolution;

					Param(const Manager::MaterialManager& _materialMgr, Shadow::ShadowAtlasMap<ShadowType>& _shadowMap, uint32 _shadowIndex, uint32 _atlasMapResolution)
						: materialMgr(_materialMgr), shadowMap(_shadowMap), shadowIndex(_shadowIndex), atlasMapResolution(_atlasMapResolution) { }
				};

				using ConstBuffersType = Shadow::Buffer::DirectionalLightShadowMapCBPool::ConstBuffers;
				static void Render(Device::DirectX& dx, Param&& param,
					const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam,
					const ConstBuffersType& shadowMapVPMatCB);
			};
		}
	}
}