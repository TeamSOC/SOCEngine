#pragma once

#include "InjectRadianceUtility.h"
#include "ComputeShader.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadianceFromSpotLight final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint32 dimension);
			void Inject(Device::DirectX& dx, VoxelMap& outVoxelMap,
						const Manager::LightManager& lightMgr,
						const Renderer::ShadowSystem& shadowParam,
						const InjectRadianceFormUtility::BindParam& bindParam);

		private:
			Shader::ComputeShader	_shader;
			uint32					_threadLength = 0;
		};
	}
}