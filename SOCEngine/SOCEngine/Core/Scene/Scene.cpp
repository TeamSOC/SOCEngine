#include "Scene.h"
#include "Director.h"

#include "EngineShaderFactory.hpp"

using namespace Core;
using namespace std;
using namespace Structure;
using namespace Math;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::PostProcessing;
using namespace Rendering::TBDR;
using namespace Rendering::Shader;
using namespace UI::Manager;
using namespace Rendering::Camera;
using namespace Rendering::Texture;
using namespace Rendering::Shadow;

Scene::Scene(void) : 
	_cameraMgr(nullptr), _uiManager(nullptr),
	_renderMgr(nullptr), _backBufferMaker(nullptr),
	_shadowRenderer(nullptr)
{
	_state = State::Init;
}

Scene::~Scene(void)
{
	SAFE_DELETE(_cameraMgr);
	SAFE_DELETE(_renderMgr);
	SAFE_DELETE(_uiManager);
	SAFE_DELETE(_lightManager);	
	SAFE_DELETE(_materialMgr);
	SAFE_DELETE(_backBufferMaker);
	SAFE_DELETE(_shadowRenderer);
}

void Scene::Initialize()
{
	_cameraMgr		= new CameraManager;

	_renderMgr		= new RenderManager;
	_renderMgr->Initialize();

	_uiManager		= new UIManager;

	_dx				= Device::Director::SharedInstance()->GetDirectX();

	_lightManager	= new LightManager;	
	_lightManager->InitializeAllShaderResourceBuffer();

	_materialMgr	= new MaterialManager;

	_backBufferMaker = new BackBufferMaker;
	_backBufferMaker->Initialize(false);

	_shadowRenderer = new ShadowRenderer;
	_shadowRenderer->Initialize(false);

	uint value = 0xff7fffff;
	float fltMin = (*(float*)&value);

	value = 0x7f7fffff;
	float fltMax = (*(float*)&value);

	_boundBox.SetMinMax(Vector3(fltMax, fltMax, fltMax), Vector3(fltMin, fltMin, fltMin));
	Matrix::Identity(_localMat);

	NextState();
	OnInitialize();
}

void Scene::Update(float dt)
{
	OnUpdate(dt);

	auto end = _rootObjects.GetVector().end();
	for(auto iter = _rootObjects.GetVector().begin(); iter != end; ++iter)
		(*iter)->Update(dt);
}

void Scene::RenderPreview()
{
	OnRenderPreview();

	Vector3 boundBoxMin = _boundBox.GetMin();
	Vector3 boundBoxMax = _boundBox.GetMax();

	const auto& objectVector = _rootObjects.GetVector();
	for(auto iter = objectVector.begin(); iter != objectVector.end(); ++iter)
		(*iter)->UpdateTransformCB_With_ComputeSceneMinMaxPos(_dx, boundBoxMin, boundBoxMax, _localMat);

	_boundBox.SetMinMax(boundBoxMin, boundBoxMax);

	Math::Matrix invViewportMat;
	_dx->GetInvViewportMatrix(invViewportMat);
	_shadowRenderer->ComputeAllLightViewProj(invViewportMat);
	_lightManager->ComputeDirectionalLightViewProj(_boundBox, invViewportMat);

	_shadowRenderer->UpdateConstBuffer(_dx);

	auto materials = _materialMgr->GetMaterials().GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		(*iter)->UpdateConstBuffer(_dx);

	auto FetchLightIndex = [&](const Light::LightForm* light) -> uint
	{
		return _lightManager->FetchLightIndexInEachLights(light);
	};
	auto FetchShadowIndex = [&](const Light::LightForm* light) -> uint
	{
		return _shadowRenderer->FetchShadowIndexInEachShadowLights(light);
	};

	_lightManager->UpdateSRBuffer(_dx, FetchShadowIndex);
	_shadowRenderer->UpdateSRBuffer(_dx, FetchLightIndex);

	const std::vector<CameraForm*>& cameras = _cameraMgr->GetCameraVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
		(*iter)->CullingWithUpdateCB(_dx, _rootObjects.GetVector(), _lightManager);
}

void Scene::Render()
{
	_dx->ClearDeviceContext();
	const RenderManager* renderMgr = _renderMgr;
	_shadowRenderer->RenderShadowMap(_dx, renderMgr);

	_dx->ClearDeviceContext();
	const std::vector<CameraForm*>& cameras = _cameraMgr->GetCameraVector();
	for(auto iter = cameras.begin(); iter != cameras.end(); ++iter)
	{
		if( (*iter)->GetUsage() == CameraForm::Usage::MeshRender )
		{
			const Buffer::ConstBuffer* shadowCB = _shadowRenderer->IsWorking() ? _shadowRenderer->GetShadowGlobalParamConstBuffer() : nullptr;
			dynamic_cast<MeshCamera*>(*iter)->Render(_dx, _renderMgr, _lightManager, shadowCB, _shadowRenderer->GetNeverUseVSM());
		}
		else if( (*iter)->GetUsage() == CameraForm::Usage::UI )
			dynamic_cast<UICamera*>(*iter)->Render(_dx);
	}
	CameraForm* mainCam = _cameraMgr->GetMainCamera();
	if(mainCam)
	{
		ID3D11RenderTargetView* backBufferRTV	= _dx->GetBackBufferRTV();
		const RenderTexture* camRT				= mainCam->GetRenderTarget();

		MeshCamera* mainMeshCam = dynamic_cast<MeshCamera*>(mainCam);
		_backBufferMaker->Render(backBufferRTV, camRT, nullptr, mainMeshCam->GetTBRParamConstBuffer());
	}

	_dx->GetSwapChain()->Present(0, 0);
	OnRenderPost();
}

void Scene::Destroy()
{
	OnDestroy();
	DeleteAllObject();

	_materialMgr->DeleteAll();
	_backBufferMaker->Destroy();
	_shadowRenderer->Destroy();
	_renderMgr->Destroy();
	_uiManager->Destroy();
	_lightManager->Destroy();
	_cameraMgr->Destroy();
}

void Scene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}

void Scene::StopState()
{
	_state = State::Stop;
}

void Scene::AddObject(Core::Object* object)
{
	_rootObjects.Add(object->GetName(), object);
}

Core::Object* Scene::FindObject(const std::string& key)
{
	Core::Object** objAddr = _rootObjects.Find(key);
	return objAddr ? (*objAddr) : nullptr;
}

void Scene::DeleteObject(Core::Object* object)
{
	std::string key = object->GetName();
	Core::Object** objectAddr = _rootObjects.Find(key);
	if(objectAddr)
	{
		Core::Object* object = (*objectAddr);
		SAFE_DELETE(object);

		_rootObjects.Delete(key);
	}
}

void Scene::DeleteAllObject()
{
	auto& objects = _rootObjects.GetVector();
	for(auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		Core::Object* object = (*iter);
		SAFE_DELETE(object);
	}

	_rootObjects.DeleteAll();
}

void Scene::Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(_state == State::Loop)
		OnInput(mouse, keyboard);
}