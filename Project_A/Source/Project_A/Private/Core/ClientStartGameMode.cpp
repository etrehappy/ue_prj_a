
#include "Core/ClientStartGameMode.h"
#include "Core/GameInstanceBase.h"

#include "ProjectNetworkSettings.h"
#include "ProjectALog.h"


static FString UrlEncode(const FString& In)
{
	FString Out = In;
	Out = Out.Replace(TEXT("%"), TEXT("%25"));
	Out = Out.Replace(TEXT("&"), TEXT("%26"));
	Out = Out.Replace(TEXT("+"), TEXT("%2B"));
	Out = Out.Replace(TEXT(" "), TEXT("%20"));
	Out = Out.Replace(TEXT("?"), TEXT("%3F"));
	Out = Out.Replace(TEXT("="), TEXT("%3D"));
	return Out;
}


EServerWorldType AClientStartGameMode::GetMapIdentifier() const
{
	return EServerWorldType::ClientStart;
}

void AClientStartGameMode::ConnectToHubServer()
{
	UGameInstanceBase* GI = Cast<UGameInstanceBase>(GetGameInstance());
	if (!GI || !GI->IsAuthenticated())
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: Cannot connect — not authenticated.", FString(__FUNCTION__));
		return;
	}

	if (HubServerAddress.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — No target world address set (HubServerAddress is empty)", FString(__FUNCTION__));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) { return; }

	const FString EncAccountId = UrlEncode(GI->GetAccountId());
	const FString EncSessionToken = UrlEncode(GI->GetSessionToken());

	const FString URL = FString::Printf(
		TEXT("%s?AccountId=%s?SessionToken=%s"),
		*HubServerAddress,
		*EncAccountId,
		*EncSessionToken
	);

	PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}

void AClientStartGameMode::ConnectToLobbyServer()
{
	/*if (IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Running Dedicated Server, skipping this", FString(__FUNCTION__));
		return;
	}

	UE_LOGFMT(LogProjectA, Log, "{0} - Connecting to lobby server...", FString(__FUNCTION__));
	UGameplayStatics::OpenLevel(this, FName(NetSet::LobbyServerAddress), true);*/
}