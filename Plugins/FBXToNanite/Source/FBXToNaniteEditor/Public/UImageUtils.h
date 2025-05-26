#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

class UImageUtils
{
public:
	static UTexture2D* LoadTextureFromFile(const FString& FilePath);
};