#include "FBXImporterLibrary.h"
#include "Factories/FbxFactory.h"
#include "Factories/FbxImportUI.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "EditorFramework/AssetImportData.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "AssetImportTask.h"
#include "Editor.h"
#include "HAL/PlatformProcess.h"
#include "Factories/FbxStaticMeshImportData.h"

UStaticMesh* FFBXImporterLibrary::ImportFBXFile(const FString& FBXPath, const FString& DestinationPath)
{
    // Generate unique asset name
    const FString AssetName = FPaths::GetBaseFilename(FBXPath);
    const FString UniquePackagePath = GenerateUniqueAssetName(AssetName, DestinationPath);

    // Create and configure FBX factory
    UFbxFactory* FbxFactory = CreateFBXFactory();
    if (!FbxFactory)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FBX factory"));
        return nullptr;
    }

    // Setup import task
    UAssetImportTask* ImportTask = NewObject<UAssetImportTask>();
    ImportTask->Filename = FBXPath;
    ImportTask->DestinationPath = UniquePackagePath;
    ImportTask->DestinationName = FPaths::GetBaseFilename(UniquePackagePath);
    ImportTask->bAutomated = true;
    ImportTask->bSave = true;
    ImportTask->bReplaceExisting = false;
    ImportTask->Factory = FbxFactory;

    // Execute import
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.ImportAssetTasks({ ImportTask });

    // Wait for asset to appear in registry
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FString ObjectPath = UniquePackagePath / ImportTask->DestinationName + TEXT(".") + ImportTask->DestinationName;
    FSoftObjectPath SoftObjectPath(ObjectPath);

    const float TimeoutSeconds = 5.0f;
    float WaitTime = 0.0f;
    FAssetData AssetData;
    
    while (!(AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(SoftObjectPath)).IsValid() && WaitTime < TimeoutSeconds)
    {
        FPlatformProcess::Sleep(0.1f);
        WaitTime += 0.1f;
    }

    if (!AssetData.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find imported asset after waiting: %s"), *ObjectPath);
        FbxFactory->RemoveFromRoot();
        return nullptr;
    }

    // Get imported asset
    UObject* ImportedAsset = AssetData.GetAsset();
    UE_LOG(LogTemp, Display, TEXT("Import completed: %s"), *ImportedAsset->GetName());

    // Cleanup and return
    FbxFactory->RemoveFromRoot();
    return Cast<UStaticMesh>(ImportedAsset);
}

UFbxFactory* FFBXImporterLibrary::CreateFBXFactory()
{
    UFbxFactory* FbxFactory = NewObject<UFbxFactory>();
    if (!FbxFactory)
    {
        return nullptr;
    }

    FbxFactory->AddToRoot();
    ConfigureFBXImportSettings(FbxFactory);
    return FbxFactory;
}

void FFBXImporterLibrary::ConfigureFBXImportSettings(UFbxFactory* Factory)
{
    if (!Factory)
    {
        return;
    }

    UFbxImportUI* ImportUI = NewObject<UFbxImportUI>();
    ImportUI->AddToRoot();
    
    ImportUI->MeshTypeToImport = FBXIT_StaticMesh;
    ImportUI->bImportMaterials = false;
    ImportUI->bImportTextures = false;
    ImportUI->bImportAnimations = false;
    ImportUI->bImportAsSkeletal = false;

    // Activamos la generación automática de colisiones para static meshes
    if (ImportUI->StaticMeshImportData)
    {
        ImportUI->StaticMeshImportData->bAutoGenerateCollision = true;
    }
    
    Factory->ImportUI = ImportUI;
}

FString FFBXImporterLibrary::GenerateUniqueAssetName(const FString& BaseName, const FString& BasePath)
{
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    FString OutPackagePath, OutAssetName;
    AssetToolsModule.Get().CreateUniqueAssetName(BasePath / BaseName, TEXT(""), OutPackagePath, OutAssetName);
    return OutPackagePath;
}