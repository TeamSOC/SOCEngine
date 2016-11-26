#pragma once

#include "Vector3.h"

namespace Rendering
{
	namespace GI
	{
		struct VXGIStaticInfo
		{
			unsigned int	dimension;
			float			maxMipLevel;
			float			voxelSize;
			float			diffuseHalfConeMaxAngle;
		};
		struct VXGIDynamicInfo
		{
			Math::Vector3	startCenterWorldPos;
			uint			packedNumfOfLights;
		};
	}
}