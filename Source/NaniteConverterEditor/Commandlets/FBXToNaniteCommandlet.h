#pragma once

#include "Commandlets/Commandlet.h"
#include "FBXToNaniteCommandlet.generated.h"

UCLASS()
class NANITECONVERTEREDITOR_API UFBXToNaniteCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UFBXToNaniteCommandlet();

    // Punto de entrada del commandlet
    virtual int32 Main(const FString& Params) override;
};