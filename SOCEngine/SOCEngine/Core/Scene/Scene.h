#pragma once

#include "Structure.h"
#include "Object.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "BufferManager.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "LightManager.h"

#include "MeshCamera.h"
#include "BackBufferMaker.h"
#include "ShadowRenderer.h"
#include "GlobalIllumination.h"

namespace Core
{
	class Scene
	{
	public:
		enum class State{ Init = 0, Loop, End, Stop, Num };

	private:
		Structure::VectorMap<std::string, Core::Object*>	_rootObjects;	

		Rendering::Manager::CameraManager*					_cameraMgr;
		Rendering::Manager::RenderManager*					_renderMgr;	
		UI::Manager::UIManager*								_uiManager;
		Rendering::Manager::LightManager*					_lightManager;
		Rendering::Manager::MaterialManager*				_materialMgr;
		Rendering::PostProcessing::BackBufferMaker*			_backBufferMaker;
		const Device::DirectX*								_dx;
		Rendering::Shadow::ShadowRenderer*					_shadowRenderer;
		Rendering::GI::GlobalIllumination*					_globalIllumination;

		State							_state;
		Intersection::BoundBox			_boundBox;
		Math::Matrix					_localMat;
		std::function<void()>			_exitFunc;

	public:
		Scene(void);
		virtual ~Scene(void);

	public:
		void Initialize();
		void Update(float dt);
		void RenderPreview();
		void Render();
		void Destroy();
		void Input(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard);

	protected:
		virtual void OnInitialize() = 0;
		virtual void OnRenderPreview() = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard) = 0;		
		virtual void OnRenderPost() = 0;
		virtual void OnDestroy() = 0;

	protected:
		void AddObject(Core::Object* object);
		Core::Object* FindObject(const std::string& key);
		void DeleteObject(Core::Object* object);
		void DeleteAllObject();

		void ActivateGI(bool activate);

	public:
		void NextState();
		void StopState();

		GET_ACCESSOR(State, State, _state);

		GET_ACCESSOR(CameraManager,		Rendering::Manager::CameraManager*,		_cameraMgr);
		GET_ACCESSOR(RenderManager,		Rendering::Manager::RenderManager*,		_renderMgr);
		GET_ACCESSOR(UIManager,			UI::Manager::UIManager*,				_uiManager);
		GET_ACCESSOR(LightManager,		Rendering::Manager::LightManager*,		_lightManager);
		GET_ACCESSOR(MaterialManager,	Rendering::Manager::MaterialManager*,	_materialMgr);
		GET_ACCESSOR(ShadowManager,		Rendering::Shadow::ShadowRenderer*,		_shadowRenderer);
	};
}