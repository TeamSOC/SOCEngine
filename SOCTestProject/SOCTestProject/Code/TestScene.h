#pragma once

#include <IScene.h>
#include <Engine.h>

class TestScene : public Core::IScene
{
public:
	virtual RenderSetting RegistRenderSetting(Core::Engine& engine, Core::EngineUtility& util)		override;

	virtual void OnInitialize(Core::Engine&, Core::EngineUtility&)		override;
	virtual void OnDestroy(Core::Engine&, Core::EngineUtility&)			override;
	virtual void OnRenderPreview(Core::Engine&, Core::EngineUtility&)	override;
	virtual void OnUpdate(Core::Engine&, Core::EngineUtility&)			override;
	virtual void OnRenderPost(Core::Engine&, Core::EngineUtility&)		override;
	virtual void OnInput(Core::Engine&, Core::EngineUtility&)			override;
};

