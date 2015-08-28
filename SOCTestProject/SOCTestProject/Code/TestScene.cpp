#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"
#include "ResourceManager.h"

using namespace Rendering;
using namespace Core;
using namespace Rendering::Camera;
using namespace Resource;
using namespace Device;
using namespace Importer;
using namespace Math;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	Object* camObj = new Object("Default");
	DefaultCamera* cam = camObj->AddComponent<DefaultCamera>();
	camObj->GetTransform()->UpdateDirection(Vector3(0, 0, 1));

	MeshImporter importer;
	importer.Initialize();
	Object* sphere = importer.Load("./Resource/sphere.obj", "./Resource/");
	sphere->GetTransform()->UpdatePosition(Vector3(0, 0, 10));
	
	AddObject(sphere);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
}

void TestScene::OnUpdate(float dt)
{
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}