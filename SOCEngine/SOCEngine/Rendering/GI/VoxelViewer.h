#pragma once

#include "VoxelMap.h"
#include "Object.h"
#include "ConstBuffer.h"
#include "ComputeShader.h"
#include "CPUReadBuffer.h"
#include "MaterialManager.h"

namespace Core
{
	class Scene;
}

namespace Rendering
{
	namespace GI
	{
		namespace Debug
		{
			class VoxelViewer
			{
			public:
				struct InfoCB
				{
					uint curCascade;
					uint dimension;

					uint dummy1;
					uint dummy2;
				};

			private:
				std::vector<Core::Object*>				_voxelObjects;
				Buffer::CPUReadBuffer*					_readBuffer;
				uint									_dimension;
				Buffer::ConstBuffer*					_infoCB;
				GPGPU::DirectCompute::ComputeShader*	_shader;
				Core::Object*							_voxelsParent;
				bool									_isAnisotropic;

			public:
				VoxelViewer();
				~VoxelViewer();

			public:
				void Initialize(uint dimension, bool isAnisotropic);
				Core::Object* GenerateVoxelViewer(const Device::DirectX* dx, ID3D11UnorderedAccessView* uav,
													uint cascade, bool realloc, float voxelizeSize, Manager::MaterialManager* matMgr);
				void DestroyAllVoxelMeshes();

			public:
				GET_ACCESSOR(VoxelsParent, Core::Object*, _voxelsParent);
			};
		}
	}
}