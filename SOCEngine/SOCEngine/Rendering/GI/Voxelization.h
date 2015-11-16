#pragma once

#include "VoxelMap.h"
#include <functional>
#include "Vector3.h"
#include "ConstBuffer.h"
#include "MeshCamera.h"

namespace Rendering
{
	namespace GI
	{
		class Voxelization
		{
		public:
			struct Info
			{
				float voxelizeSize;
				uint  dimension;
			};

			struct InfoCBData : public Info
			{
				float voxelSize;
				uint  currentCascade;
			};

			struct ViewProjAxisesCBData
			{
				Math::Matrix viewProjX;
				Math::Matrix viewProjY;
				Math::Matrix viewProjZ;
			};

		private:
			std::vector<VoxelMap*>	_voxelColorMaps;
			std::vector<VoxelMap*>	_voxelNormalMaps;

			Buffer::ConstBuffer*	_infoConstBuffer;
			Buffer::ConstBuffer*	_viewProjAxisesConstBuffer;

			InfoCBData				_initVoxelizationInfo;
			bool					_changedInitVoxelizationInfo;

			Math::Matrix			_prevViewMat;

			uint					_numOfCascades;

		public:
			Voxelization();
			~Voxelization();

		public:
			void Initialize(uint cascades, float minWorldSize = 4.0f, uint dimension = 256);
			void Destroy();

		public:
			void Clear(Device::DirectX* dx);
			void Voxelize(Device::DirectX* dx, const Camera::MeshCamera* camera);

		public:
			void UpdateInitVoxelizationInfo(const Info& info);
		};
	}
}