#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"
#include "ResourceManager.h"

#include "MeshImporter.h"

using namespace Rendering;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Light;
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
	camera = new Object("Default");
	MainCamera* cam = camera->AddComponent<MainCamera>();
	camera->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));
	camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));

	MeshImporter importer;
	testObject = importer.Load("./Resources/tt.obj");
//	testObject = importer.Load("./Resources/TurretsPack/turret_1.FBX");

//	testObject->GetTransform()->UpdatePosition(Vector3(0, -0.5f, 1));
	testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 2.0f));

	testObject->GetTransform()->UpdateEulerAngles(Vector3(45, 35, 20));
	AddObject(testObject);

	light = new Object("Light");
	light->AddComponent<DirectionalLight>();
	light->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));

	AddObject(light);
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnInput(const Device::Win32::Mouse& mouse, const  Device::Win32::Keyboard& keyboard)
{
	if(keyboard.states['W'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(0, 10, 0));
	}
	if(keyboard.states['A'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(-10, 0, 0));
	}
	if(keyboard.states['S'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(0, -10, 0));
	}
	if(keyboard.states['D'] == Win32::Keyboard::Type::Up)
	{
		Vector3 pos = testObject->GetTransform()->GetLocalPosition();
		testObject->GetTransform()->UpdatePosition(pos + Vector3(10, 0, 0));
	}

	if(keyboard.states['Y'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(10, 0, 0));
	}
	if(keyboard.states['G'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(0, -10, 0));
	}
	if(keyboard.states['H'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(-10, 0, 0));
	}
	if(keyboard.states['J'] == Win32::Keyboard::Type::Up)
	{
		Vector3 e = testObject->GetTransform()->GetLocalEulerAngle();
		testObject->GetTransform()->UpdateEulerAngles(e + Vector3(0, 10, 0));
	}
}

void TestScene::OnUpdate(float dt)
{
	//static float x = 0.0f;

	//x += 0.1f;
	//testObject->GetTransform()->UpdateEulerAngles(Math::Vector3(45, x, 0));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}