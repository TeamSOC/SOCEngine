#include "DirectionalLightShadow.h"
#include "DirectionalLight.h"
#include <assert.h>

using namespace Rendering::Shadow;
using namespace Rendering::Light;
using namespace Math;
using namespace Core;
using namespace Intersection;

Matrix DirectionalLightShadow::MakeMatrixParam() const
{
	return Matrix::Transpose(_viewProjMat);
}

void DirectionalLightShadow::ComputeViewProjMatrix(
	const LightPool<DirectionalLight>& lightPool,
	const TransformPool& tfPool,
	const BoundBox& sceneBoundBox)
{
	assert(_base.GetDirty());

	auto light = lightPool.Find(_base.GetLightId().Literal());
	assert(light);

	const auto& lightBase = light->GetBase();

	auto transform = tfPool.Find(lightBase.GetObjectId().Literal());
	assert(transform);


	Matrix view = transform->GetWorldMatrix();

	if (_useAutoProjectLocation)
	{
		Vector3 forward = transform->GetWorldForward();
		const Vector3& sceneCenter = sceneBoundBox.GetCenter();

		view._41 = sceneCenter.x - (forward.x * FrustumMaxZ / 2.0f);
		view._42 = sceneCenter.y - (forward.y * FrustumMaxZ / 2.0f);
		view._43 = sceneCenter.z - (forward.z * FrustumMaxZ / 2.0f);
		view._44 = 1.0f;
	}

	view = Matrix::ComputeViewMatrix(view);

	float orthogonalWH = (_projectionSize < FLT_EPSILON) ? sceneBoundBox.GetSize().Length() : _projectionSize;
	Matrix proj = Matrix::OrthoLH(orthogonalWH, orthogonalWH, FrustumMaxZ, FrustumMinZ);

	// Done!
	_viewProjMat = view * proj;

	// Compute Param
	{
		proj = Matrix::Inverse(proj);

		_param.invProj_34 = proj._34;
		_param.invProj_44 = proj._44;
	}
}
