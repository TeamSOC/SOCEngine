#include "MeshUtility.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Intersection;
using namespace Rendering::Manager;
using namespace Core;
using namespace Math;

void MeshUtility::Culling(const Frustum& frustum, MeshManager& meshMgr, const TransformPool& transformPool)
{
	auto Cull = [&frustum, &meshMgr, &transformPool](auto& pool) -> void
	{
		uint size = pool.GetSize();
		for (uint meshIDx = 0; meshIDx < size; ++meshIDx)
		{
			auto& mesh = pool.Get(meshIDx);

			ObjectID id = mesh.GetObjectID();
			const Transform* transform = transformPool.Find(id.Literal());
			Vector3 worldPos = transform->GetWorldPosition();

			mesh._culled = frustum.In(worldPos, mesh.GetRadius());
		}
	};

	Cull( meshMgr.GetPool<OpaqueTrait>() );
	Cull( meshMgr.GetPool<TransparencyTrait>() );
	Cull( meshMgr.GetPool<AlphaBlendTrait>() );
}
