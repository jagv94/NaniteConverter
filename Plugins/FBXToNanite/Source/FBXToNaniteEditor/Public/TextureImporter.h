#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

class UTexture2D;

class FBXTONANITEEDITOR_API FTextureImporter
{
public:
    static UTexture2D* ImportTexture(const FString& FilePath, const FString& PackagePath, const FString& TextureName);
    
private:
    static bool LoadImageFile(const FString& FilePath, TArray<uint8>& OutRawData, int32& OutWidth, int32& OutHeight);
    static UTexture2D* CreateTextureAsset(const FString& PackagePath, const FString& TextureName, 
                                         const TArray<uint8>& PixelData, int32 Width, int32 Height);
};