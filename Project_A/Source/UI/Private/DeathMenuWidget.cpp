#include "DeathMenuWidget.h"

#include "Components/Button.h"

#include "ProjectALog.h"

void UDeathMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!RespawnButton || !ExitButton)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - One or more buttons are nullptr", FString(__FUNCTION__));
	}

	RespawnButton->OnClicked.AddDynamic(this, &UDeathMenuWidget::HandleRespawnClicked);
	ExitButton->OnClicked.AddDynamic(this, &UDeathMenuWidget::HandleExitClicked);
}

void UDeathMenuWidget::NativeDestruct()
{
	if (!RespawnButton || !ExitButton)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - One or more buttons are nullptr", FString(__FUNCTION__));
	}

	RespawnButton->OnClicked.RemoveDynamic(this, &UDeathMenuWidget::HandleRespawnClicked);
	ExitButton->OnClicked.RemoveDynamic(this, &UDeathMenuWidget::HandleExitClicked);

	Super::NativeDestruct();
}

void UDeathMenuWidget::HandleRespawnClicked()
{
	OnRespawnRequested.Broadcast();
}

void UDeathMenuWidget::HandleExitClicked()
{
	OnExitRequested.Broadcast();
}
