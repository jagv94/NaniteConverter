#include "FBXToNaniteImporter.h"
#include "Engine/StaticMesh.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "UObject/SavePackage.h"

UStaticMesh* FFBXToNaniteImporter::ImportFBX(const FString& FBXPath)
{
    FScopedSlowTask ImportTask(4, FText::FromString("Importing FBX to Nanite"));
    ImportTask.MakeDialog();

    // Validate file exists
    ImportTask.EnterProgressFrame(1, FText::FromString("Validating FBX File"));
    if (!FPaths::FileExists(FBXPath))
    {
        UE_LOG(LogTemp, Error, TEXT("FBX file not found: %s"), *FBXPath);
        return nullptr;
    }

    const FString AssetName = FPaths::GetBaseFilename(FBXPath);
    const FString BasePackagePath = TEXT("/Game/FBX_Imported");

    // Import FBX
    ImportTask.EnterProgressFrame(1, FText::FromString("Importing FBX Mesh"));
    UStaticMesh* StaticMesh = FFBXImporterLibrary::ImportFBXFile(FBXPath, BasePackagePath);
    if (!StaticMesh)
    {
        return nullptr;
    }

    // Setup Nanite
    ImportTask.EnterProgressFrame(1, FText::FromString("Configuring Nanite"));
    SetupNaniteForStaticMesh(StaticMesh);

    // Setup materials
    ImportTask.EnterProgressFrame(1, FText::FromString("Setting Up Materials"));
    const FString MaterialPackagePath = BasePackagePath / AssetName / TEXT("Materials");
    if (!UEditorAssetLibrary::DoesDirectoryExist(MaterialPackagePath))
    {
        UEditorAssetLibrary::MakeDirectory(MaterialPackagePath);
    }

    SetupMaterialsForStaticMesh(StaticMesh, FBXPath, MaterialPackagePath, AssetName);

    // Create temp map with the mesh
    const FString TempMapName = FString::Printf(TEXT("_StartupMap_Temp_%s"), *AssetName);
    FMapEditor::DuplicateAndModifyMap(StaticMesh, TEXT("/Game/_StartupMap"), TempMapName);

    return StaticMesh;
}

FTextureSet FFBXToNaniteImporter::DetectAndLoadTextures(const FString& ModelPath, const FString& AssetName,
    const FString& MaterialPackagePath)
{
    FTextureSet TextureSet;
    FString BaseName = AssetName;

    // Handle transformed assets
    if (AssetName.EndsWith(TEXT("_TRANSFORMED")))
    {
        BaseName = AssetName.LeftChop(11);
    }

    // Look for texture folder
    FString FBMFolder = FPaths::GetPath(ModelPath) / (BaseName + TEXT(".fbm"));
    if (!FPaths::DirectoryExists(FBMFolder))
    {
        UE_LOG(LogTemp, Warning, TEXT("Texture folder not found: %s"), *FBMFolder);
        return TextureSet;
    }

    // Find all JPG textures
    TArray<FString> TextureFiles;
    IFileManager::Get().FindFiles(TextureFiles, *(FBMFolder / TEXT("*.jpg")), true, false);

    // Load detected textures
    for (const FString& TexFile : TextureFiles)
    {
        FString FullTexPath = FBMFolder / TexFile;

        if (TexFile == BaseName + TEXT(".jpg"))
        {
            TextureSet.BaseColor = FTextureImporter::ImportTexture(
                FullTexPath,
                MaterialPackagePath,
                TEXT("T_") + AssetName + TEXT("_BaseColor")
            );
        }
        else if (TexFile == BaseName + TEXT("1.jpg"))
        {
            TextureSet.Normal = FTextureImporter::ImportTexture(
                FullTexPath,
                MaterialPackagePath,
                TEXT("T_") + AssetName + TEXT("_Normal")
            );
        }
        else if (TexFile == BaseName + TEXT("2.jpg"))
        {
            TextureSet.ORM = FTextureImporter::ImportTexture(
                FullTexPath,
                MaterialPackagePath,
                TEXT("T_") + AssetName + TEXT("_ORM")
            );
        }
    }

    return TextureSet;
}

void FFBXToNaniteImporter::ApplyTransformedMaterials(UStaticMesh* StaticMesh, const FString& FBXPath,
    const FString& MaterialPackagePath, const FString& AssetName)
{
    if (!StaticMesh) return;

    FString FBMFolder = FPaths::GetPath(FBXPath) / (AssetName + TEXT(".fbm"));
    TArray<FString> TextureFiles;
    IFileManager::Get().FindFiles(TextureFiles, *(FBMFolder / TEXT("*.jpg")), true, false);

    if (TextureFiles.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No se encontraron texturas en: %s"), *FBMFolder);
        return;
    }

    // Marca que vas a modificar el StaticMesh
    StaticMesh->Modify();

    // Importa texturas, crea materiales y asigna a los slots
    for (int32 i = 0; i < TextureFiles.Num(); ++i)
    {
        FString TextureFileName = TextureFiles[i];
        FString FullTexturePath = FBMFolder / TextureFileName;
        FString TextureBaseName = FPaths::GetBaseFilename(TextureFileName);

        FString MaterialName = TEXT("M_") + TextureBaseName;
        FString TextureName = TEXT("T_") + TextureBaseName;

        UTexture2D* Texture = FTextureImporter::ImportTexture(FullTexturePath, MaterialPackagePath, TextureName);

        if (Texture)
        {
            UMaterial* Material = FMaterialCreator::CreateMaterial(
                MaterialName,
                MaterialPackagePath,
                FTextureSet{ Texture, nullptr, nullptr }
            );

            if (Material)
            {
                if (i < StaticMesh->GetStaticMaterials().Num())
                {
                    StaticMesh->SetMaterial(i, Material);
                    UE_LOG(LogTemp, Display, TEXT("Material asignado: %s a slot %d"), *Material->GetName(), i);
                }
                else
                {
                    // Si hay más materiales que slots, añadir nuevo slot
                    FStaticMaterial NewStaticMaterial(Material);
                    StaticMesh->GetStaticMaterials().Add(NewStaticMaterial);
                    UE_LOG(LogTemp, Warning, TEXT("Nuevo slot creado y asignado material: %s"), *Material->GetName());
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No se pudo crear material para textura: %s"), *TextureFileName);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No se pudo importar textura: %s"), *FullTexturePath);
        }
    }

    // Indicar que los materiales del mesh van a cambiar
    StaticMesh->PreEditChange(nullptr);
    StaticMesh->PostEditChange();
    StaticMesh->MarkPackageDirty();

    // Guardar paquete (opcional)
    FString MeshPackagePath = StaticMesh->GetOutermost()->GetName();
    FString MeshFilePath = FPackageName::LongPackageNameToFilename(MeshPackagePath, FPackageName::GetAssetPackageExtension());

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.SaveFlags = SAVE_None;
    SaveArgs.Error = GError;

    if (!UPackage::SavePackage(StaticMesh->GetOutermost(), StaticMesh, *MeshFilePath, SaveArgs))
    {
        UE_LOG(LogTemp, Warning, TEXT("Error guardando StaticMesh en: %s"), *MeshFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("StaticMesh guardado en: %s"), *MeshFilePath);
    }
}

void FFBXToNaniteImporter::SetupNaniteForStaticMesh(UStaticMesh* StaticMesh)
{
    if (!StaticMesh) return;

    StaticMesh->PreEditChange(nullptr);
    StaticMesh->NaniteSettings.bEnabled = true;
    StaticMesh->Build(false);
    StaticMesh->PostEditChange();
    StaticMesh->MarkPackageDirty();
}

void FFBXToNaniteImporter::SetupMaterialsForStaticMesh(UStaticMesh* StaticMesh, const FString& FBXPath,
    const FString& MaterialPackagePath, const FString& AssetName)
{
    if (AssetName.EndsWith(TEXT("_TRANSFORMED")))
    {
        ApplyTransformedMaterials(StaticMesh, FBXPath, MaterialPackagePath, AssetName);
    }
    else
    {
        FTextureSet TextureSet = DetectAndLoadTextures(FBXPath, AssetName, MaterialPackagePath);
        UMaterial* Material = FMaterialCreator::CreateMaterial(AssetName, MaterialPackagePath, TextureSet);

        if (Material)
        {
            for (int32 i = 0; i < StaticMesh->GetStaticMaterials().Num(); i++)
            {
                StaticMesh->SetMaterial(i, Material);
            }
            StaticMesh->MarkPackageDirty();
        }
    }
}