#include "FBXToNaniteCommandlet.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "FBXToNaniteImporter.h" // tu clase importadora
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogFBXToNaniteCommandlet, Log, All);

UFBXToNaniteCommandlet::UFBXToNaniteCommandlet()
{
    IsClient = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UFBXToNaniteCommandlet::Main(const FString& Params)
{
    FString FBXPath;
    FParse::Value(*Params, TEXT("FBXPath="), FBXPath);

    if (FBXPath.IsEmpty() || !FPaths::FileExists(FBXPath))
    {
        UE_LOG(LogFBXToNaniteCommandlet, Error, TEXT("Debes proporcionar una ruta v�lida al archivo FBX usando -FBXPath=\"Ruta\\Archivo.fbx\""));
        return 1;
    }

    UE_LOG(LogFBXToNaniteCommandlet, Display, TEXT("Importando FBX desde: %s"), *FBXPath);

    FFBXToNaniteImporter Importer;
    UStaticMesh* ImportedMesh = Importer.ImportFBX(FBXPath);  // Aseg�rate de que ImportFBX acepte ruta como par�metro

    UE_LOG(LogFBXToNaniteCommandlet, Display, TEXT("Importaci�n finalizada."));

    return 0;
}