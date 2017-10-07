#pragma once

#include "VectorIndexer.hpp"
#include "Mesh.h"
#include "ObjectID.hpp"
#include <tuple>
#include <assert.h>
#include "VBSortedMeshes.hpp"

namespace Rendering
{
	namespace Geomtry
	{
		using TransparentMeshPool	= Core::VectorHashMap<Core::ObjectID::LiteralType, Mesh>;
		using TransparentMeshPtrs	= std::vector<const Mesh*>;

		using OpaqueMeshPool		= VBSortedMeshes<MeshRawPool>;
		using OpaqueMeshPtrs		= VBSortedMeshes<MeshRawPtrs>;

		using AlphaBlendMeshPool	= OpaqueMeshPool;
		using AlphaBlendMeshPtrs	= OpaqueMeshPtrs;		
	};
	
	namespace Manager
	{
		class MeshManager final
		{
		public:
			MeshManager() = default;
			DISALLOW_ASSIGN_COPY(MeshManager);

		public:
			Geometry::Mesh& Add(Geometry::Mesh& mesh, Geometry::TransparentMeshPool& meshPool)
			{
				assert(mesh.GetVBKey() != 0); //Error, mesh does not init yet.
				return meshPool.Add(mesh.GetObjectID().Literal(), mesh);
			}

			Geometry::Mesh& Add(Geometry::Mesh& mesh, Geometry::OpaqueMeshPool& meshPool) // or AlphaBlendMeshPool
			{
				assert(mesh.GetVBKey() != 0); //Error, mesh does not init yet.				
				return meshPool.Add(mesh.GetObjectID(), mesh.GetVBKey(), mesh);
			}			
			
			template<class Pool> void Delete(Core::ObjectID objID, Pool& meshPool)
			{
				meshPool.Delete(objID.Literal());
			}
			template<class Pool> bool Has(Core::ObjectID objID, Pool& meshPool) const
			{
				return	meshPool.Has(objID.Literal());
			}
			template<class Pool> Geometry::Mesh* Find(Core::ObjectID id, Pool& meshPool)
			{
				return meshPool.Find(id.Literal());
			}
			template<class Pool> const Geometry::Mesh* Find(Core::ObjectID id, Pool& meshPool) const
			{
				return meshPool.Find(id.Literal());
			}			

			Geometry::Mesh& Add(Geometry::Mesh& mesh)
			{
				return Add(mesh, GetOpaqueMeshPool());
			}
			
			void Delete(Core::ObjectID objID);
			bool Has(Core::ObjectID objID) const;
			Geometry::Mesh* Find(Core::ObjectID id);

			void CheckDirty(const Core::TransformPool& tfPool);
			void ComputeWorldSize(Math::Vector3& refWorldMin, Math::Vector3& refWorldMax, const Core::TransformPool& tfPool) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::TransformPool& tfPool);

			void ClearDirty() { _dirtyMeshes.clear(); }

			bool ChangeTrait(Core::ObjectID id,
							 Geometry::OpaqueMeshPool& fromPool, Geometry::TransparentMeshPool& toPool)
			{
				uint literlID = id.Literal();
				assert(fromPool.Has(literlID));
				assert(toPool.Has(literlID) == false);

				toPool.Add(literlID, *fromPool.Find(literlID));
				fromPool.Delete(literlID);
			}
			bool ChangeTrait(Core::ObjectID id,
							 Geometry::TransparentMeshPool& fromPool, Geometry::OpaqueMeshPool& toPool) // or AlphaBlend
			{
				uint literlID = id.Literal();
				assert(fromPool.Has(literlID));
				assert(toPool.Has(literlID) == false);

				Mesh* mesh = fromPool.Find(literlID);
				toPool.Add(literlID, mesh->GetVBKey(), *mesh);
				fromPool.Delete(literlID);
			}

			GET_ACCESSOR(TransparentMeshPool,	Geometry::TransparentMeshPool&,				_transparentMeshPool);
			GET_ACCESSOR(OpaqueMeshPool,		Geometry::OpaqueMeshPool&,					_opaqueMeshPool);
			GET_ACCESSOR(AlphaBlendMeshPool,	Geometry::AlphaBlendMeshPool&,				_alphaBlendMeshPool);

			GET_CONST_ACCESSOR(TransparentMeshPool,	const Geometry::TransparentMeshPool&,	_transparentMeshPool);
			GET_CONST_ACCESSOR(OpaqueMeshPool,		const Geometry::OpaqueMeshPool&,		_opaqueMeshPool);
			GET_CONST_ACCESSOR(AlphaBlendMeshPool,	const Geometry::AlphaBlendMeshPool&,	_alphaBlendMeshPool);

			GET_CONST_ACCESSOR(HasDirtyMeshes, bool, _dirtyMeshes.empty() == false);

		private:
			Geometry::TransparentMeshPool	_transparentMeshPool;
			Geometry::OpaqueMeshPool		_opaqueMeshPool;
			Geometry::AlphaBlendMeshPool	_alphaBlendMeshPool;

			std::vector<Geometry::Mesh*> _dirtyMeshes;
		};
	}
}
