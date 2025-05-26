#include "TextureImporter.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Modules/ModuleManager.h"
#include "UObject/SavePackage.h"

UTexture2D* FTextureImporter::ImportTexture(const FString& FilePath, const FString& PackagePath, const FString& TextureName)
{
    // Validate input parameters
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Texture file not found: %s"), *FilePath);
        return nullptr;
    }

    if (PackagePath.IsEmpty() || TextureName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid package path or texture name"));
        return nullptr;
    }

    // Load image data
    TArray<uint8> RawFileData;
    int32 Width = 0;
    int32 Height = 0;
    
    if (!LoadImageFile(FilePath, RawFileData, Width, Height))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load image file: %s"), *FilePath);
        return nullptr;
    }

    // Create texture asset
    UTexture2D* NewTexture = CreateTextureAsset(PackagePath, TextureName, RawFileData, Width, Height);
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create texture asset"));
        return nullptr;
    }

    return NewTexture;
}

bool FTextureImporter::LoadImageFile(const FString& FilePath, TArray<uint8>& OutRawData, int32& OutWidth, int32& OutHeight)
{
    FScopedSlowTask SlowTask(3, FText::FromString("Loading Image File"));
    SlowTask.MakeDialog();

    // Load file to byte array
    SlowTask.EnterProgressFrame(1);
    if (!FFileHelper::LoadFileToArray(OutRawData, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load file to array: %s"), *FilePath);
        return false;
    }

    // Detect image format
    SlowTask.EnterProgressFrame(1);
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(OutRawData.GetData(), OutRawData.Num());
    
    if (ImageFormat == EImageFormat::Invalid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Unsupported image format: %s"), *FPaths::GetExtension(FilePath));
        return false;
    }

    // Create image wrapper and decompress
    SlowTask.EnterProgressFrame(1);
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(OutRawData.GetData(), OutRawData.Num()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to decompress image: %s"), *FilePath);
        return false;
    }

    // Get raw image data
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, OutRawData))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get raw BGRA data: %s"), *FilePath);
        return false;
    }

    OutWidth = ImageWrapper->GetWidth();
    OutHeight = ImageWrapper->GetHeight();

    return true;
}

UTexture2D* FTextureImporter::CreateTextureAsset(const FString& PackagePath, const FString& TextureName, 
                                                 const TArray<uint8>& PixelData, int32 Width, int32 Height)
{
    FScopedSlowTask SlowTask(3, FText::FromString("Creating Texture Asset"));
    SlowTask.MakeDialog();

    const FString FullAssetPath = PackagePath / TextureName;

    // Create package
    SlowTask.EnterProgressFrame(1);
    UPackage* TexturePackage = CreatePackage(*FullAssetPath);
    if (!TexturePackage)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create texture package"));
        return nullptr;
    }
    TexturePackage->FullyLoad();

    // Create texture object
    SlowTask.EnterProgressFrame(1);
    UTexture2D* NewTexture = NewObject<UTexture2D>(
        TexturePackage, 
        *TextureName, 
        RF_Public | RF_Standalone | RF_MarkAsRootSet
    );

    if (!NewTexture)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create texture object"));
        return nullptr;
    }

    // Configure texture
    NewTexture->MipGenSettings = TMGS_NoMipmaps;
    NewTexture->CompressionSettings = TC_Default;
    NewTexture->SRGB = true;
    NewTexture->AddressX = TA_Clamp;
    NewTexture->AddressY = TA_Clamp;

    // Initialize with image data
    SlowTask.EnterProgressFrame(1);
    NewTexture->Source.Init(
        Width,
        Height,
        1,      // NumSlices
        1,      // NumMips
        TSF_BGRA8,
        PixelData.GetData()
    );

    // Update resource and mark dirty
    NewTexture->UpdateResource();
    NewTexture->MarkPackageDirty();

    // Register with asset registry
    FAssetRegistryModule::AssetCreated(NewTexture);

    // Save package
    FString PackageFilename = FPackageName::LongPackageNameToFilename(
        FullAssetPath, 
        FPackageName::GetAssetPackageExtension()
    );

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.Error = GError;

    if (!UPackage::SavePackage(TexturePackage, NewTexture, *PackageFilename, SaveArgs))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save texture package: %s"), *PackageFilename);
        return nullptr;
    }

    return NewTexture;
}