#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class SpotLightShadow : public ShadowCommon
		{
		public:
			SpotLightShadow(const Light::LightForm* owner);
			~SpotLightShadow();

		public:
			void ComputeViewProjMatrix();

		public:
			void MakeMatrixParam(Math::Matrix& outViewProjMat) const;
		};
	}
}