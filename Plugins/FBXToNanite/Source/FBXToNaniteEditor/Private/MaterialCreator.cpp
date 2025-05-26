#include "MaterialCreator.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "MaterialEditingLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/FbxImportUI.h"
#include "Factories/MaterialFactoryNew.h"
#include "EditorAssetLibrary.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "PackageTools.h"

UMaterial* FMaterialCreator::CreateMaterial(const FString& MaterialName, const FString& SavePath, const FTextureSet& TextureSet)
{
    if (SavePath.IsEmpty() || MaterialName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Parámetros inválidos para la creación del material"));
        return nullptr;
    }

    FString CleanAssetName = MaterialName;
    FString PackagePath = SavePath / CleanAssetName;
    FName AssetName = FName(*CleanAssetName);

    // Si existe, eliminar
    if (UEditorAssetLibrary::DoesAssetExist(PackagePath))
    {
        if (!UEditorAssetLibrary::DeleteAsset(PackagePath))
        {
            UE_LOG(LogTemp, Warning, TEXT("No se pudo eliminar el material existente: %s"), *PackagePath);
            return nullptr;
        }
    }

    // Crear paquete
    UPackage* MaterialPackage = CreatePackage(*PackagePath);
    if (!MaterialPackage)
    {
        UE_LOG(LogTemp, Error, TEXT("No se pudo crear el paquete del material"));
        return nullptr;
    }

    MaterialPackage->FullyLoad();

    // Crear material
    UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
    UMaterial* NewMaterial = Cast<UMaterial>(MaterialFactory->FactoryCreateNew(
        UMaterial::StaticClass(),
        MaterialPackage,
        AssetName,
        RF_Public | RF_Standalone | RF_MarkAsRootSet,
        nullptr,
        GWarn
    ));

    if (!NewMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("Fallo al crear el material"));
        return nullptr;
    }

    // Crear nodos
    if (TextureSet.BaseColor)
    {
        SetupBaseColorNode(NewMaterial, TextureSet.BaseColor);
    }

    if (TextureSet.Normal)
    {
        SetupNormalMapNode(NewMaterial, TextureSet.Normal);
    }

    // Activar Nanite (opcional, depende de cómo lo uses)
    EnableNaniteUsage(NewMaterial);

    // Actualizar cambios
    NewMaterial->PreEditChange(nullptr);
    NewMaterial->PostEditChange();
    NewMaterial->MarkPackageDirty();

    // Registrar asset
    FAssetRegistryModule::AssetCreated(NewMaterial);

    // Guardar
    FString MatFilePath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.SaveFlags = SAVE_None;
    SaveArgs.Error = GError;

    bool bSaved = UPackage::SavePackage(MaterialPackage, NewMaterial, *MatFilePath, SaveArgs);

    if (!bSaved)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error al guardar material: %s"), *MatFilePath);
        return nullptr;
    }

    UE_LOG(LogTemp, Display, TEXT("Material guardado en: %s.%s"), *PackagePath, *CleanAssetName);
    UE_LOG(LogTemp, Display, TEXT("Ruta del archivo guardado: %s"), *MatFilePath);

    return NewMaterial;
}

void FMaterialCreator::EnableNaniteUsage(UMaterial* Material)
{
    if (!Material) return;

    Material->Modify();
    Material->bUsedWithNanite = true;
    Material->PostEditChange();
    Material->MarkPackageDirty();

    SaveMaterialPackage(Material);
}

void FMaterialCreator::SetupBaseColorNode(UMaterial* Material, UTexture2D* BaseColorTex)
{
    if (!Material || !BaseColorTex) return;

    // Coordenadas
    UMaterialExpressionTextureCoordinate* TexCoord =
        Cast<UMaterialExpressionTextureCoordinate>(
            UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionTextureCoordinate::StaticClass())
        );
    TexCoord->MaterialExpressionEditorX = -600;
    TexCoord->MaterialExpressionEditorY = 0;

    // Texture Sample
    UMaterialExpressionTextureSample* ColorSample =
        Cast<UMaterialExpressionTextureSample>(
            UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionTextureSample::StaticClass())
        );

    ColorSample->Texture = BaseColorTex;
    ColorSample->SamplerType = SAMPLERTYPE_Color;
    ColorSample->MaterialExpressionEditorX = -400;
    ColorSample->MaterialExpressionEditorY = 0;
    ColorSample->Coordinates.Expression = TexCoord;

    UMaterialEditingLibrary::ConnectMaterialProperty(ColorSample, TEXT("RGB"), MP_BaseColor);
}

void FMaterialCreator::SetupNormalMapNode(UMaterial* Material, UTexture2D* NormalTex)
{
    if (!Material || !NormalTex) return;

    UMaterialExpressionTextureSample* NormalSample =
        Cast<UMaterialExpressionTextureSample>(
            UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionTextureSample::StaticClass())
        );

    NormalSample->Texture = NormalTex;
    NormalSample->SamplerType = SAMPLERTYPE_Normal;
    NormalSample->MaterialExpressionEditorX = -400;
    NormalSample->MaterialExpressionEditorY = 200;

    UMaterialEditingLibrary::ConnectMaterialProperty(NormalSample, TEXT("RGB"), MP_Normal);
}

bool FMaterialCreator::SaveMaterialPackage(UMaterial* Material)
{
    if (!Material) return false;

    const FString PackageFilename = FPackageName::LongPackageNameToFilename(
        Material->GetOutermost()->GetName(),
        FPackageName::GetAssetPackageExtension()
    );

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.SaveFlags = SAVE_None;
    SaveArgs.Error = GError;

    return UPackage::SavePackage(
        Material->GetOutermost(),
        Material,
        *PackageFilename,
        SaveArgs
    );
}