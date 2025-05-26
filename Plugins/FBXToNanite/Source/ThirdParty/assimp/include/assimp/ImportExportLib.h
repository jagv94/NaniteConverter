#ifndef IMPORTEXPORTLIBRARY_H
#define IMPORTEXPORTLIBRARY_H

#ifdef IMPORTEXPORTLIBRARY_EXPORTS
#define IMPORTEXPORTLIBRARY_API __declspec(dllexport)
#else
#define IMPORTEXPORTLIBRARY_API __declspec(dllimport)
#endif

#include "UE_Type.h"

//assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

#include <string>
#include <vector>

class IMPORTEXPORTLIBRARY_API ImportExportLib
{
public:
	ImportExportLib();
	~ImportExportLib();

public:
	const aiScene* ImportSceneExport(Assimp::Importer *mImporter, std::string fileName, UE_Type::UETransform & transform);

	const aiScene* ImportScene(Assimp::Importer* mImporter, std::string fileName, UE_Type::UETransform& transform);

	int ExportFBXModel(std::vector<std::string> modelPaths, std::vector<UE_Type::UETransform> transforms, std::string outputPath);

};

#endif // IMPORTEXPORTLIBRARY_H

