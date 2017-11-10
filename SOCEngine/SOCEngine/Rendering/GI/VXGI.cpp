#include "VXGI.h"
#include "BindIndexInfo.h"
#include "ShaderFactory.hpp"


using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace Rendering::Factory;
using namespace Rendering::Renderer;

void VXGI::InitializeClearVoxelMap(DirectX& dx, ShaderManager& shaderMgr, uint dimension)
{
	ShaderFactory factory(&shaderMgr);
	_clearVoxelMap = *factory.LoadComputeShader(dx, "ClearVoxelMap", "CS", nullptr, "@ClearVoxelMap");
}

void VXGI::ClearInjectColorVoxelMap(DirectX& dx)
{
	auto Clear = [&dx, &shader = _clearVoxelMap](UnorderedAccessView& uav, uint sideLength, bool isAnisotropic)
	{
		auto ComputeThreadGroupSideLength = [](uint sideLength)
		{
			return static_cast<uint>( static_cast<float>(sideLength + 8 - 1) / 8.0f );
		};

		ComputeShader::ThreadGroup threadGroup(0, 0, 0);
		{
			threadGroup.x = ComputeThreadGroupSideLength(sideLength * (isAnisotropic ? (uint)VoxelMap::Direction::Num : 1));
			threadGroup.y = ComputeThreadGroupSideLength(sideLength);
			threadGroup.z = ComputeThreadGroupSideLength(sideLength);
		}
		
		shader.SetThreadGroupInfo(threadGroup);

		ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex(0), uav);
		shader.Dispatch(dx);
		ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex(0));
	};

	Clear(_injectionSourceMap.GetSourceMapUAV(), _staticInfo.dimension, false);
	Clear(_mipmappedInjectionMap.GetSourceMapUAV(), _staticInfo.dimension / 2, true);
}


void VXGI::Initialize(DirectX& dx, ShaderManager& shaderMgr, const VXGIStaticInfo& info)
{

	// Setting Infos
	{
		_staticInfo = info;
		_infoCB.staticInfoCB.Initialize(dx);
		_infoCB.staticInfoCB.UpdateSubResource(dx, info);

		_infoCB.dynamicInfoCB.Initialize(dx);
		_infoCB.dynamicInfoCB.UpdateSubResource(dx, VXGIDynamicInfo());
	}

	// Init Voxelization
	_voxelization.Initialize(dx, shaderMgr, _staticInfo.dimension, _staticInfo.voxelSize);

	// Injection
	{
		_injectionSourceMap.Initialize(dx, _staticInfo.dimension, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, 1, false);
		_mipmappedInjectionMap.Initialize(dx, _staticInfo.dimension / 2, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, uint(_staticInfo.maxMipLevel) - 1, true);

		_injectPointLight.Initialize(dx, shaderMgr, _staticInfo.dimension);
		_injectSpotLight.Initialize(dx, shaderMgr, _staticInfo.dimension);
	}

	// Mipmap
	_mipmap.Initialize(dx, shaderMgr);

	// Voxel Cone Tracing
	_voxelConeTracing.Initialize(dx, shaderMgr);

	InitializeClearVoxelMap(dx, shaderMgr, _staticInfo.dimension);
}

void VXGI::Run(DirectX& dx, VXGI::Param&& param)
{	
	ClearInjectColorVoxelMap(dx);
	
	const auto& lightMgr		= param.lightMgr;
	const auto& shadowSystem	= param.shadowSystem;
	const auto& tbrCB			= param.main.renderer.GetTBRParamCB();

	// 1. Voxelization Pass
	{
		// Clear Voxel Map and voxelize
		_voxelization.Voxelize(dx, _injectionSourceMap,
			Voxelization::Param{_startCenterWorldPos, _infoCB, lightMgr, shadowSystem,
								tbrCB, param.cullParam, param.meshRenderParam, param.materialMgr}
		);
	}

	// 2. Injection Pass
	{
		InjectRadianceFormUtility::BindParam param{
			_infoCB, _voxelization,  tbrCB,
			shadowSystem.manager.GetGlobalParamCB()
		};

		if(lightMgr.GetLightCount<PointLight>() > 0)
			_injectPointLight.Inject(dx, _injectionSourceMap, lightMgr, shadowSystem, param);

		if(lightMgr.GetLightCount<SpotLight>() > 0)
			_injectSpotLight.Inject(dx, _injectionSourceMap, lightMgr, shadowSystem, param);
	}

	// 3. Mipmap Pass
	_mipmap.Mipmapping(dx, _injectionSourceMap, _mipmappedInjectionMap);

	// 4. Voxel Cone Tracing Pass
	_voxelConeTracing.Run(dx, _injectionSourceMap, _mipmappedInjectionMap, _infoCB, param.main);
}

void VXGI::UpdateGIDynamicInfoCB(DirectX& dx, uint packedNumfOfLights)
{
	VXGIDynamicInfo info;
	info.startCenterWorldPos	= _startCenterWorldPos;
	info.packedNumfOfLights		= packedNumfOfLights;

	_infoCB.dynamicInfoCB.UpdateSubResource(dx, info);
}