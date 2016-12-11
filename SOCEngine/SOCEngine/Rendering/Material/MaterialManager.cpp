#include "MaterialManager.h"
#include "PhysicallyBasedMaterial.h"

using namespace Rendering;
using namespace Rendering::Manager;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Initialize()
{
	PhysicallyBasedMaterial* material = new PhysicallyBasedMaterial("@Default");
	material->Initialize();
	material->SetMainColor(Color(1.0f, 1.0f, 1.0f, 1.0f));

	Add("@Default", material);
}

void MaterialManager::Add(const std::string& file, const std::string& name, Material* material)
{
	std::string key = file + ":" + name;
	Add(key, material);
}

Material* MaterialManager::Find(const std::string& file, const std::string& name)
{
	std::string key = file + ":" + name;
	return Find(key);
}

void MaterialManager::Delete(const std::string& file, const std::string& name)
{
	std::string key = file  + ":" + name;
	Delete(key);
}

void MaterialManager::DeleteAll()
{
	auto materials = _materials.GetVector();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		SAFE_DELETE(*iter);

	_materials.DeleteAll();
}

void MaterialManager::Add(const std::string& key, Material* material)
{
	ASSERT_MSG_IF(Find(key) == nullptr, "Error, Duplicated key");
	_materials.Add(key, material);
}

Material* MaterialManager::Find(const std::string& key)
{
	Material** found = _materials.Find(key);
	return found ? (*found) : nullptr;
}

void MaterialManager::Delete(const std::string& key)
{
	Material** found = _materials.Find(key);
	if(found)
	{
		SAFE_DELETE(*found);
		_materials.Delete(key);
	}
}
