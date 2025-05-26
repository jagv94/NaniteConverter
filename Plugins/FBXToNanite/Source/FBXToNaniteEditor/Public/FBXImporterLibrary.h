#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"

class UStaticMesh;
class UFbxFactory;

class FBXTONANITEEDITOR_API FFBXImporterLibrary
{
public:
    static UStaticMesh* ImportFBXFile(const FString& FBXPath, const FString& DestinationPath);
    
private:
    static UFbxFactory* CreateFBXFactory();
    static void ConfigureFBXImportSettings(UFbxFactory* Factory);
    static FString GenerateUniqueAssetName(const FString& BaseName, const FString& BasePath);
};