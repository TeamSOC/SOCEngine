#include "DirectionalLightShadow.h"
#include "DirectionalLight.h"

using namespace Rendering::Shadow;
using namespace Rendering::Light;

DirectionalLightShadow::DirectionalLightShadow(const LightForm* owner)
	: ShadowCommon(owner)
{
	SetBias(0.0f);
}

DirectionalLightShadow::~DirectionalLightShadow()
{
}

void DirectionalLightShadow::MakeMatrixParam(Math::Matrix& outViewProjMat, Math::Matrix& outInvVPVMat) const
{
	const DirectionalLight* owner = dynamic_cast<const DirectionalLight*>(_owner);

#ifdef USE_SHADOW_INVERTED_DEPTH
	if(GetUseVSM())
		Math::Matrix::Transpose(outViewProjMat,	owner->GetInvNearFarViewProjectionMatrix());
	else
#endif
	Math::Matrix::Transpose(outViewProjMat,		owner->GetViewProjectionMatrix());
	Math::Matrix::Transpose(outInvVPVMat,		owner->GetInvViewProjViewportMatrix());
}