#pragma once

#include "CoreMinimal.h"
#include "Materials/Material.h"
#include "Engine/Texture2D.h"

class UMaterial;
class UTexture2D;

struct FTextureSet
{
    UTexture2D* BaseColor = nullptr;
    UTexture2D* Normal = nullptr;
    UTexture2D* ORM = nullptr;
};

class FBXTONANITEEDITOR_API FMaterialCreator
{
public:
    static UMaterial* CreateMaterial(const FString& MaterialName, const FString& SavePath, 
                                    const FTextureSet& TextureSet);
    static void EnableNaniteUsage(UMaterial* Material);
    
private:
    static void SetupBaseColorNode(UMaterial* Material, UTexture2D* BaseColorTex);
    static void SetupNormalMapNode(UMaterial* Material, UTexture2D* NormalTex);
    static bool SaveMaterialPackage(UMaterial* Material);
};