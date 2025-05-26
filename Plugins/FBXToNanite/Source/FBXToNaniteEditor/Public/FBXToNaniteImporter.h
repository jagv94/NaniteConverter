#pragma once

#include "CoreMinimal.h"
#include "TextureImporter.h"
#include "MaterialCreator.h"
#include "FBXImporterLibrary.h"
#include "MapEditor.h"

class UStaticMesh;
class UMaterial;

class FBXTONANITEEDITOR_API FFBXToNaniteImporter
{
public:
    // Main import function
    UStaticMesh* ImportFBX(const FString& FBXPath);

private:
    // Texture handling
    FTextureSet DetectAndLoadTextures(const FString& ModelPath, const FString& AssetName, const FString& MaterialPackagePath);

    // Material handling
    void ApplyTransformedMaterials(UStaticMesh* StaticMesh, const FString& FBXPath,
        const FString& MaterialPackagePath, const FString& AssetName);
    void SetupMaterialsForStaticMesh(UStaticMesh* StaticMesh, const FString& FBXPath,
        const FString& MaterialPackagePath, const FString& AssetName);

    // Nanite configuration
    void SetupNaniteForStaticMesh(UStaticMesh* StaticMesh);
};