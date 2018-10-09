#pragma once

#include "ConstBuffer.h"

namespace Rendering
{
	namespace Renderer
	{
		class ShadowAtlasMapRenderer;
	}

	namespace Manager
	{
		class ShadowManager;
	}

	namespace Shadow
	{
		namespace Buffer
		{
			struct ShadowGlobalParamCBData
			{
				uint32 packedNumOfShadowAtlasCapacity;
				uint32 packedPowerOfTwoShadowResolution;
				uint32 packedNumOfShadows;
				uint32 dummy;
			};

			class ShadowGlobalParamCB final 
				: public Rendering::Buffer::ExplicitConstBuffer<ShadowGlobalParamCBData>
			{
			public:
				void UpdateSubResource(Device::DirectX& dx, const Manager::ShadowManager& manager, const Renderer::ShadowAtlasMapRenderer& renderer);

			private:
				using Parent = Rendering::Buffer::ExplicitConstBuffer<ShadowGlobalParamCBData>;
				using Parent::UpdateSubResource;
			};
		}
	}
}
