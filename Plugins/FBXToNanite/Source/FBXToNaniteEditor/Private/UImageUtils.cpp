#include "UImageUtils.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "HAL/FileManager.h"
#include "Logging/LogMacros.h"

UTexture2D* UImageUtils::LoadTextureFromFile(const FString& FilePath)
{
    // Validar que el archivo exista
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Archivo no encontrado: %s"), *FilePath);
        return nullptr;
    }

    // Cargar módulo ImageWrapper para soporte de distintos formatos de imagen
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

    // Leer el archivo en un array de bytes
    TArray<uint8> RawFileData;
    if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Error cargando archivo: %s"), *FilePath);
        return nullptr;
    }

    // Detectar extensión para crear el wrapper adecuado
    FString Extension = FPaths::GetExtension(FilePath).ToLower();
    TSharedPtr<IImageWrapper> ImageWrapper;

    if (Extension == TEXT("jpg") || Extension == TEXT("jpeg"))
    {
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
    }
    else if (Extension == TEXT("png"))
    {
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
    }
    else if (Extension == TEXT("bmp"))
    {
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Formato de imagen no soportado: %s"), *Extension);
        return nullptr;
    }

    // Decodificar la imagen
    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo decodificar la imagen: %s"), *FilePath);
        return nullptr;
    }

    TArray<uint8> UncompressedBGRA;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo obtener datos RAW BGRA de la imagen: %s"), *FilePath);
        return nullptr;
    }

    // Crear la textura transitoria
    const int32 Width = ImageWrapper->GetWidth();
    const int32 Height = ImageWrapper->GetHeight();

    UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    if (!Texture)
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo crear la textura transitoria"));
        return nullptr;
    }

    // Bloquear el mip 0 para copiar los datos
    void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

    // Ajustes finales para la textura
    Texture->SRGB = true;
    Texture->UpdateResource();

    return Texture;
}