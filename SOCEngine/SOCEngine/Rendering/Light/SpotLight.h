#pragma once

#include "LightForm.h"
#include "Cone.h"
#include "Vector2.h"
#include "SpotLightShadow.h"

namespace Rendering
{
	namespace Light
	{
		class SpotLight : public LightForm
		{
		public:
			struct Param
			{
				unsigned short			dirX;
				unsigned short			dirY;

				unsigned short			outerConeCosAngle;
				unsigned short			innerConeCosAngle;

				Param() : outerConeCosAngle(0), innerConeCosAngle(0), dirX(0), dirY(0) {}
				~Param() {}
			};

		public:
			Param			_param;
			float			_spotAngleDegree;

		public:
			SpotLight();
			virtual ~SpotLight();

		public:
			virtual void CreateShadow();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Param& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			void SetSpotAngleDegree(float d);

			GET_ACCESSOR(SpotAngleDegree,	float,						_spotAngleDegree);
			GET_ACCESSOR(Shadow,			Shadow::SpotLightShadow*,	static_cast<Shadow::SpotLightShadow*>(_shadow) );
		};
	}
}
