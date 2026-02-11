


#include "GeneralHud.h"

#include "GeneralWidget.h"
#include "Blueprint/UserWidget.h"

#include "ProjectALog.h"

void AGeneralHud::BeginPlay()
{
	Super::BeginPlay();

	CreateMainWidget();
}

bool AGeneralHud::CreateMainWidget()
{
	if (MainWidget)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - MainWidget already created", FString(__FUNCTION__));
		return true;
	}

	if (!GeneralWidgetClass)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - GeneralWidgetClass is not set", FString(__FUNCTION__));
		return false;
	}
		
	const auto PlayerController = GetOwningPlayerController();
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Error, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return false;
	}		

	MainWidget = CreateWidget<UGeneralWidget>(PlayerController, GeneralWidgetClass);
	if (!MainWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create MainWidget", FString(__FUNCTION__));
		return false;
	}

	MainWidget->AddToViewport();

	return true;
}

void AGeneralHud::ToggleInventory()
{
	if (!MainWidget)
	{
		if (!CreateMainWidget())
		{
			UE_LOGFMT(LogProjectA, Log, "{0} - MainWidget is not available", FString(__FUNCTION__));
			return;
		}
	}

	MainWidget->ToggleInventoryVisibility();
}
