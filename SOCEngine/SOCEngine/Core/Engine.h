#pragma once

#include "Common.h"
#include "IScene.h"
#include <memory>
#include <chrono>
#include "ComponentSystem.h"
#include "ObjectManager.h"
#include "RootObjectIDs.hpp"

#include "Singleton.h"
#include "Rect.h"

#include "RenderingSystem.h"
#include "MeshImporter.h"

namespace Device
{
	class DirectX;
}

namespace Core
{
	class Engine final
	{
	public:
		Engine(Device::DirectX& dx);

		DISALLOW_ASSIGN(Engine);
		DISALLOW_COPY_CONSTRUCTOR(Engine);

		// Scene
		void RunScene();
		void ChangeScene(IScene* scene);

		// System
		void Initialize(IScene* scene);
		void Render();
		void Destroy();

		// Importer
		Object* LoadMesh(const std::string& fileDir, bool useDynamicVB = false, bool useDynamicIB = false);

		// Root
		void AddRootObject(const Core::Object& object);

		GET_ACCESSOR_REF(DirectX,			_dx);
		GET_ACCESSOR_REF(ComponentSystem,	_componentSystem);
		GET_ACCESSOR_REF(TransformPool,		_transformPool);
		GET_ACCESSOR_REF(RootObjectIDs,		_rootObjectIDs);
		GET_ACCESSOR_REF(RenderingSystem,	_rendering);
		GET_ACCESSOR_REF(ObjectManager,		_objectManager);
		GET_ACCESSOR_REF(Importer,			_importer);

		GET_CONST_ACCESSOR(Exit,	bool,	_exit);

	private:
		NullScene									_nullScene;
		IScene*										_scene;

		clock_t										_prevTime	= 0;
		clock_t										_lag		= 0;

		Core::RootObjectIDs							_rootObjectIDs;

		Core::ObjectManager							_objectManager;
		Device::DirectX&							_dx;
		Core::ComponentSystem						_componentSystem;
		Core::TransformPool							_transformPool;
		Rendering::RenderingSystem					_rendering;
		Importer::MeshImporter						_importer;

	private:
		std::vector<Core::Transform*>				_dirtyTransforms;
		bool										_exit = false;
	};
}