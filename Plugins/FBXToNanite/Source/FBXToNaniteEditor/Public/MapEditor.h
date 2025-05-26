#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

class UStaticMesh;
class UWorld;

class FBXTONANITEEDITOR_API FMapEditor
{
public:
    static void DuplicateAndModifyMap(UStaticMesh* StaticMesh, const FString& SourceMapPath, 
                                     const FString& TempMapName);
    
private:
    static FString GetCurrentMapPath();
    static bool DuplicateMapAsset(const FString& SourcePath, const FString& DestPath);
    static bool LoadMap(const FString& MapPath);
    static void SpawnStaticMeshInWorld(UWorld* World, UStaticMesh* StaticMesh);
    static bool SaveCurrentMap();
};