#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"
#include "LightCulling.h"

#include "Director.h"
#include "ResourceManager.h"

#include "MeshImporter.h"
#include "PhysicallyBasedMaterial.h"

using namespace Rendering;
using namespace Core;
using namespace Rendering::Camera;
using namespace Rendering::Geometry;
using namespace Rendering::Light;
using namespace Rendering::Geometry;
using namespace Resource;
using namespace Device;
using namespace Math;

TestScene::TestScene(void)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	float t1 = cos(Math::Common::Deg2Rad(179.0f));
	float t2 = Math::Common::Rad2Deg(t1);

	float t3 = cos(Math::Common::Deg2Rad(0.01f));
	float t4 = Math::Common::Rad2Deg(t3);

	camera = new Object("Default");
	MainCamera* cam = camera->AddComponent<MainCamera>();
	camera->GetTransform()->UpdateEulerAngles(Vector3(0, 0, 0));
	camera->GetTransform()->UpdatePosition(Vector3(0, 0, 0));

	Importer::MeshImporter importer;
	//testObject = importer.Load("./Resources/Capsule/capsule.obj");
	//testObject->GetTransform()->UpdatePosition(Vector3(0, 0, 5));
	testObject = importer.Load("./Resources/House/SanFranciscoHouse.fbx");
	testObject->GetTransform()->UpdatePosition(Vector3(0, -5, 15));
	testObject->GetTransform()->UpdateEulerAngles(Vector3(90, 90, 0));
	AddObject(testObject);

	light = new Object("Light");
	//light->AddComponent<DirectionalLight>();
	//light->GetTransform()->UpdatePosition(Vector3(0, 0, 0));
	SpotLight* pl = light->AddComponent<SpotLight>();
	light->GetTransform()->UpdatePosition(Vector3(0, 0, -10));
	light->GetTransform()->LookAtWorld(Vector3(0, -5, 15));
	pl->SetRadius(100.0f);
	pl->SetSpotAngleDegree(15.0f);
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
	static float x = 0.0f;

	x += 0.1f;
	testObject->GetTransform()->UpdateEulerAngles(Math::Vector3(90, x, 0));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}