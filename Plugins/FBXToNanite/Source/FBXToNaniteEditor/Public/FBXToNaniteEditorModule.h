#pragma once

#include "Modules/ModuleManager.h"

class FFBXToNaniteEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};