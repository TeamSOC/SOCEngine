#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class DirectionalLightShadow : public ShadowCommon
		{
		public:
			DirectionalLightShadow(const Light::LightForm* owner);
			~DirectionalLightShadow();

		public:
			void MakeMatrixParam(Math::Matrix& outViewProjMat) const;
		};
	}
}