#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "PointLight.h"

namespace Rendering
{
	namespace Manager
	{
		class ShadowManager;
	}
	namespace Renderer
	{
		namespace ShadowMap
		{
			class PLShadowMapRenderer;
		}
	}

	namespace Shadow
	{
		namespace Buffer
		{
			class PointLightShadowBuffer;
		}

		class PointLightShadow final
		{
		public:
			using ViewProjMatType	= std::array<Math::Matrix, 6>;
			using ShadowBufferType	= Buffer::PointLightShadowBuffer;
			using LightType			= Light::PointLight;
			using ManagerType		= Rendering::Manager::ShadowManager;
			using ShadowMapRenderer	= Renderer::ShadowMap::PLShadowMapRenderer;

		public:
			PointLightShadow(BaseShadow base) : _base(base) {}
			ViewProjMatType MakeVPMatParam(const Light::LightPool<Light::PointLight>& lightPool, const Core::TransformPool& tfPool);

			GET_CONST_ACCESSOR(ViewProjectionMatrix,	const auto&,	_transposedViewProjMat);
			GET_CONST_ACCESSOR(Base,					const auto&,	_base);
			GET_CONST_ACCESSOR(ObjectID,				Core::ObjectID,	_base.GetObjectID());
			GET_CONST_ACCESSOR(Dirty,					bool,			_base.GetDirty());

			inline void SetDirty(bool b) { _base.SetDirty(b); }

		private:
			std::array<Math::Matrix, 6> 	_transposedViewProjMat;
			BaseShadow						_base;
		};
	}
}