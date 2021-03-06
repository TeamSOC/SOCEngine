#pragma once

#include "Half.h"
#include "ComputeShader.h"
#include "Transform.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "Frustum.h"
#include "RenderTexture.h"
#include "DepthMap.h"
#include "ObjectID.hpp"
#include "MeshRenderQueue.h"

#include "MaterialID.hpp"

#undef near
#undef far

namespace Rendering
{
	namespace Manager
	{
		class CameraManager;
	}

	namespace Camera
	{
		class MainCamera final
		{
		public:
			using ManagerType = Manager::CameraManager;

			struct CameraCBData
			{
				Math::Matrix	viewMat;
				Math::Matrix	viewProjMat;
				Math::Matrix	prevViewProjMat;

				Math::Vector3	worldPos			= Math::Vector3(0.0f, 0.0f, 0.0f);
				uint			packedCamNearFar	= 0;
			};
			struct Desc
			{				
				float			fieldOfViewDegree	= 45.0f;
				float			near				= 0.1f;
				float			far					= 1000.0f;
				float			aspect				= 1.0f;
				Color			clearColor			= Color::Clear();
				Rect<uint>		renderRect			= Rect<uint>(0, 0, 0, 0);
			};

		public:
			MainCamera(Core::ObjectID objID) : _objID(objID) {}
			DISALLOW_ASSIGN_COPY(MainCamera);

			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Rect<uint>& rect);
			void Destroy();

			void UpdateCB(Device::DirectX& dx, const Core::Transform& transform);
			void ClearDirty() { _dirty = false; }

			Math::Matrix ComputePerspectiveMatrix(bool isInverted) const;
			Math::Matrix ComputeOrthogonalMatrix(bool isInverted) const;

			GET_CONST_ACCESSOR(FieldOfViewDegree,		float,			_desc.fieldOfViewDegree);
			GET_CONST_ACCESSOR(Near,					float,			_desc.near);
			GET_CONST_ACCESSOR(Far,						float,			_desc.far);
			GET_CONST_ACCESSOR(ClearColor,				const Color&,	_desc.clearColor);
			SET_ACCESSOR_DIRTY(FieldOfViewDegree,		float,			_desc.fieldOfViewDegree);
			SET_ACCESSOR_DIRTY(Near,					float,			_desc.near);
			SET_ACCESSOR_DIRTY(Far,						float,			_desc.far);
			SET_ACCESSOR_DIRTY(ClearColor,				const Color&,	_desc.clearColor);
			GET_CONST_ACCESSOR(ObjectID,				Core::ObjectID,	_objID);
			SET_ACCESSOR(ObjectID,						Core::ObjectID,	_objID);
			GET_CONST_ACCESSOR(RenderRect,				const auto&,	_desc.renderRect);
			GET_CONST_ACCESSOR(ViewProjMatrix,			const auto&,	_viewProjMat);
			GET_CONST_ACCESSOR(ProjMatrix,				const auto&,	_projMat);
			GET_CONST_ACCESSOR(WorldMatrix,				const auto&,	_worldMat);
			GET_CONST_ACCESSOR(CameraCB,				const auto&,	_camCB);
			GET_CONST_ACCESSOR(Dirty,					bool,			_dirty);
			GET_CONST_ACCESSOR(HasChangedCB,			bool,			_hasChangedCB);			
			GET_CONST_ACCESSOR(Frustum,					const auto&,	_frustum);
			GET_CONST_ACCESSOR(TransparentMeshRenderQ,	const auto&,	_transparentMeshRenderQ);
			GET_CONST_ACCESSOR(OpaqueMeshRenderQ,		const auto&,	_opaqueMeshRenderQ);
			GET_CONST_ACCESSOR(AlphaTestMeshRenderQ,	const auto&,	_alphaTestMeshRenderQ);

			GET_CONST_ACCESSOR(SkyBoxMaterialID,		MaterialID,		_skyBoxMaterialID);
			SET_ACCESSOR(SkyBoxMaterialID,				MaterialID,		_skyBoxMaterialID);

			void ClassifyMesh(const Geometry::MeshPoolPtrs&& pool, const Core::ObjectManager& objMgr, const Core::TransformPool& transformPool);

		private:
			Buffer::ExplicitConstBuffer<CameraCBData>	_camCB;
			CameraCBData								_camCBData;

			Math::Matrix								_worldMat;
			Math::Matrix								_projMat;
			Math::Matrix								_viewProjMat;

			Math::Matrix								_prevViewProjMat;
			Intersection::Frustum						_frustum;
			TransformCB::ChangeState					_camCBChangeState = TransformCB::ChangeState::HasChanged;
			Desc										_desc;

			RenderQueue::TransparentMeshRenderQueue		_transparentMeshRenderQ;
			RenderQueue::OpaqueMeshRenderQueue			_opaqueMeshRenderQ;
			RenderQueue::AlphaTestMeshRenderQueue		_alphaTestMeshRenderQ;

			Core::ObjectID								_objID;
			MaterialID									_skyBoxMaterialID;
			bool										_dirty			= true;
			bool										_hasChangedCB	= true;
		};
	}
}
