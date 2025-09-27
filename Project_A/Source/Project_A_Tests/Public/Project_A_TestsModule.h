#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FProject_A_Tests : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
