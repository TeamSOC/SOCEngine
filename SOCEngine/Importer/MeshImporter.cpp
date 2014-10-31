#include "MeshImporter.h"
#include "Utility.h"

using namespace Importer;
using namespace Core;

MeshImporter::MeshImporter() : _objImporter(nullptr)
{
}

MeshImporter::~MeshImporter()
{
	SAFE_DELETE(_objImporter);
}

Object* MeshImporter::Load(const std::string& fileDir, const std::string& materialFolder)
{
	std::string fileName, fileExtension, folderDir;
	if( Utility::ParseDirectory(fileDir, folderDir, fileName, fileExtension) == false )
		return nullptr;

	
	Object* meshObject = nullptr;
	if(fileExtension == "obj")
		meshObject = _objImporter->Load(fileDir, fileName, materialFolder);

	return meshObject;
}