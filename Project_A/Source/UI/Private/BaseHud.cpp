
#include "BaseHud.h"

#include "ProjectALog.h"

void ABaseHud::OnConnectedSuccessfully(EServerWorldType World)
{
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	HideNetConnectionStatusWidget();

	switch (World)
	{
	case EServerWorldType::None:
		UE_LOGFMT(LogProjectA, Warning, "{0} - EServerWorldType::None", FString(__FUNCTION__));
		break;

	case EServerWorldType::Test:
		UE_LOGFMT(LogProjectA, Log, "{0} - EServerWorldType::Test", FString(__FUNCTION__));
		break;

	case EServerWorldType::Hub:
		UE_LOGFMT(LogProjectA, Log, "{0} - EServerWorldType::Hub", FString(__FUNCTION__));		
		ShowMouse();
		break;

	case EServerWorldType::MainWorld:
		UE_LOGFMT(LogProjectA, Log, "{0} - EServerWorldType::MainWorld", FString(__FUNCTION__));
		break;

	case EServerWorldType::Lobby:
		UE_LOGFMT(LogProjectA, Log, "{0} - EServerWorldType::Lobby", FString(__FUNCTION__));
		ShowMouse();
		break;

	default:
		UE_LOGFMT(LogProjectA, Warning, "{0} - EServerWorldType is not set", FString(__FUNCTION__));
		break;
	}
}

void ABaseHud::ShowMouse()
{
	APlayerController* PlayerControllerP = GetWorld()->GetFirstPlayerController();
	if (!PlayerControllerP)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is not found", FString(__FUNCTION__));
		return;
	}

	PlayerControllerP->SetShowMouseCursor(true);
	PlayerControllerP->SetInputMode(FInputModeUIOnly());
}

void ABaseHud::HideMouse()
{
	APlayerController* PlayerControllerP = GetWorld()->GetFirstPlayerController();
	if (!PlayerControllerP)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is not found", FString(__FUNCTION__));
		return;
	}

	PlayerControllerP->SetShowMouseCursor(false);
	PlayerControllerP->SetInputMode(FInputModeGameOnly());
}

void ABaseHud::HideNetConnectionStatusWidget()
{
	if (!NetConnectionStatusWidget)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - NetConnectionStatusWidget is empty", FString(__FUNCTION__));
		return;
	}

	HideMouse();
	NetConnectionStatusWidget->RemoveFromParent();
	NetConnectionStatusWidget = nullptr;

	UE_LOGFMT(LogProjectA, Log, "{0} - NetConnectionStatusWidget was set to nullptr", FString(__FUNCTION__));

}
