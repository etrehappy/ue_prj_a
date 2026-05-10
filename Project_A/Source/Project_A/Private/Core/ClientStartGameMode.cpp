


#include "Core/ClientStartGameMode.h"
#include "Core/HubGameMode.h"
#include "Character/CustomPlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "ProjectNetworkSettings.h"
#include "ProjectALog.h"

AClientStartGameMode::AClientStartGameMode()
{
}

EServerWorldType AClientStartGameMode::GetMapIdentifier() const
{
	return EServerWorldType::ClientStart;
}

void AClientStartGameMode::ConnectToHubServer()
{
	if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	UE_LOGFMT(LogProjectA, Log, "{0} - Connecting to the server...", FString(__FUNCTION__));

	UGameplayStatics::OpenLevel(this, FName(NetSet::HubServerAddress), true);
}
