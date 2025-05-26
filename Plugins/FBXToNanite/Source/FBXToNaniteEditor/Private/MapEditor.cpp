#include "MapEditor.h"
#include "Editor/UnrealEd/Public/FileHelpers.h"
#include "Editor/UnrealEd/Public/EditorLevelUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Editor.h"
#include "Components/StaticMeshComponent.h"
#include "EditorAssetLibrary.h"
#include "Misc/Paths.h"

void FMapEditor::DuplicateAndModifyMap(UStaticMesh* StaticMesh, const FString& SourceMapPath, const FString& TempMapName)
{
    if (!StaticMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("StaticMesh is null - cannot duplicate and modify map"));
        return;
    }

    // 1. Save reference to current map
    const FString OriginalMapPath = GetCurrentMapPath();
    if (!OriginalMapPath.IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("Current map: %s"), *OriginalMapPath);
    }

    // 2. Validate source map exists
    if (!UEditorAssetLibrary::DoesAssetExist(SourceMapPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Source map does not exist: %s"), *SourceMapPath);
        return;
    }

    // 3. Build destination path
    const FString TempMapPackagePath = FPaths::Combine(FPaths::GetPath(SourceMapPath), TempMapName);
    
    // 4. Duplicate the map asset
    if (!DuplicateMapAsset(SourceMapPath, TempMapPackagePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to duplicate map to: %s"), *TempMapPackagePath);
        return;
    }

    // 5. Load the duplicated map
    if (!LoadMap(TempMapPackagePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load duplicated map: %s"), *TempMapPackagePath);
        return;
    }

    // 6. Get the temporary world
    UWorld* TempWorld = GEditor->GetEditorWorldContext().World();
    if (!IsValid(TempWorld) || !IsValid(TempWorld->PersistentLevel))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid temporary world"));
        return;
    }

    // 7. Spawn static mesh in the new map
    SpawnStaticMeshInWorld(TempWorld, StaticMesh);

    // 8. Save the modified map
    if (!SaveCurrentMap())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save duplicated map"));
    }

    // 9. Restore original map if one was open
    if (!OriginalMapPath.IsEmpty())
    {
        LoadMap(OriginalMapPath);
    }
}

FString FMapEditor::GetCurrentMapPath()
{
    if (UWorld* CurrentWorld = GEditor->GetEditorWorldContext().World())
    {
        return CurrentWorld->GetOutermost()->GetName();
    }
    return FString();
}

bool FMapEditor::DuplicateMapAsset(const FString& SourcePath, const FString& DestPath)
{
    return UEditorAssetLibrary::DuplicateAsset(SourcePath, DestPath) != nullptr;
}

bool FMapEditor::LoadMap(const FString& MapPath)
{
    return FEditorFileUtils::LoadMap(MapPath, false, true);
}

void FMapEditor::SpawnStaticMeshInWorld(UWorld* World, UStaticMesh* StaticMesh)
{
    if (!World || !StaticMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid parameters for SpawnStaticMeshInWorld"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(*FString::Printf(TEXT("SM_%s"), *StaticMesh->GetName()));
    SpawnParams.OverrideLevel = World->PersistentLevel;

    AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(), 
        FTransform::Identity, 
        SpawnParams
    );

    if (MeshActor)
    {
        UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
        if (MeshComponent)
        {
            MeshComponent->SetStaticMesh(StaticMesh);
            MeshComponent->RegisterComponent();
        }
        MeshActor->SetActorLabel(StaticMesh->GetName());
        UE_LOG(LogTemp, Log, TEXT("Successfully spawned StaticMesh in map: %s"), *StaticMesh->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn StaticMeshActor"));
    }
}

bool FMapEditor::SaveCurrentMap()
{
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World || !World->PersistentLevel)
    {
        return false;
    }

    bool bSaved = FEditorFileUtils::SaveLevel(World->PersistentLevel);
    if (bSaved)
    {
        UE_LOG(LogTemp, Log, TEXT("Map saved successfully"));
    }
    return bSaved;
}