
#include "Core/HubGameMode.h"
#include "Character/CustomPlayerController.h"
#include "ProjectNetworkSettings.h"
#include "Common/UdpSocketBuilder.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "TimerManager.h"
#include "CharacterService.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"


#include "ProjectALog.h"

AHubGameMode::AHubGameMode()
{
	PlayerControllerClass = ACustomPlayerController::StaticClass();
	//DefaultPawnClass = AMyCharacter::StaticClass(); 
	// HUDClass = AMyHUD::StaticClass();    
	// GameStateClass = AMyGameState::StaticClass();        
	// PlayerStateClass = AMyPlayerState::StaticClass();
	// SpectatorClass = AMySpectatorPawn::StaticClass();
}

void AHubGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitHeartbeatListener();

	GetWorldTimerManager().SetTimer(HeartbeatPollTimerHandle, this, &AHubGameMode::PollHeartbeats, 0.2f, true);
	GetWorldTimerManager().SetTimer(HeartbeatTimeoutTimerHandle, this, &AHubGameMode::MarkTimedOutServers, 1.0f, true);
}

EServerWorldType AHubGameMode::GetMapIdentifier() const
{
	return EServerWorldType::Hub;
}

void AHubGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(HeartbeatPollTimerHandle);
	GetWorldTimerManager().ClearTimer(HeartbeatTimeoutTimerHandle);
	ShutdownHeartbeatListener();

	Super::EndPlay(EndPlayReason);
}

FString AHubGameMode::InitNewPlayer(APlayerController* NewPlayerController,
	const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	const FString AccountId = UGameplayStatics::ParseOption(Options, TEXT("AccountId"));
	const FString SessionToken = UGameplayStatics::ParseOption(Options, TEXT("SessionToken"));

	if (AccountId.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: Player connected without AccountId. Options={1}", FString(__FUNCTION__), *Options);
	}
	else
	{
		PlayerAccountIds.Add(NewPlayerController, AccountId);
		PlayerSessionTokens.Add(NewPlayerController, SessionToken);
		UE_LOGFMT(LogProjectA, Log, "{0}: Player connected. AccountId={1}", FString(__FUNCTION__), *AccountId);
	}

	return Result;
}

TArray<FWorldServerView> AHubGameMode::BuildWorldServersSnapshot() const
{
	TMap<FName, FWorldServerView> SnapshotMap{};

	for (const TPair<FName, FWorldServerView>& Pair : RuntimeServers)
	{
		SnapshotMap.Add(Pair.Key, Pair.Value);
	}

	TArray<FWorldServerView> Snapshot{};
	SnapshotMap.GenerateValueArray(Snapshot);

	//UE_LOGFMT(LogProjectA, Log, "{0} - Snapshot size={1} (Registered={2}, Runtime={3})", FString(__FUNCTION__), Snapshot.Num(), RegisteredWorldServers.Num(), RuntimeServers.Num());

	return Snapshot;
}

void AHubGameMode::InitHeartbeatListener()
{
	HeartbeatSocket = FUdpSocketBuilder(TEXT("HubHeartbeatListener"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToPort(HubHeartbeatPort)
		.WithReceiveBufferSize(2 * 1024 * 1024);

	if (!HeartbeatSocket)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to bind heartbeat socket on port {1}", FString(__FUNCTION__), HubHeartbeatPort);
	}
}

void AHubGameMode::ShutdownHeartbeatListener()
{
	if (HeartbeatSocket)
	{
		HeartbeatSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(HeartbeatSocket);
		HeartbeatSocket = nullptr;
	}
}

void AHubGameMode::PollHeartbeats()
{
	if (!HeartbeatSocket)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Heartbeat socket is not initialized", FString(__FUNCTION__));
		return;
	}

	uint32 PendingSize = 0;

	while (HeartbeatSocket->HasPendingData(PendingSize))
	{
		FString Payload;
		if (!ReceiveNextHeartbeat(PendingSize, Payload))
		{
			continue; // failed to read this datagram, skip to the next one
		}

		const TMap<FString, FString> ServerData = ParsePayloadToKV(Payload);
		TryProcessHeartbeatKV(ServerData);
	}
}

void AHubGameMode::MarkTimedOutServers()
{
	const double CurrentTime = FPlatformTime::Seconds();

	for (TPair<FName, FWorldServerView>& Pair : RuntimeServers)
	{
		const double* LastSeen = LastSeenByServerId.Find(Pair.Key);
		if (!LastSeen)
		{
			Pair.Value.bIsOnline = false;
			continue; // We haven't seen a heartbeat from this server yet, mark it as offline until we do.
		}

		bool bIsTimedOut = (CurrentTime - *LastSeen) > static_cast<double>(HeartbeatTimeoutSeconds);
		if (bIsTimedOut)
		{
			Pair.Value.bIsOnline = false; // Mark as offline if we haven't seen a heartbeat recently.
		}
	}
}

// Read one datagram 
bool AHubGameMode::ReceiveNextHeartbeat(uint32 PendingSize, FString& OutPayload)
{
	// 1. Prepare
	if (!HeartbeatSocket)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Heartbeat socket is not initialized", FString(__FUNCTION__));
		return false;
	}

	const int32 BufferSize = FMath::Min(static_cast<int32>(PendingSize), 2048);
	TArray<uint8> Buffer;
	Buffer.SetNumZeroed(BufferSize + 1); // +1 to be safe for text conversions

	int32 BytesRead = 0;
	TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// 2. Receive
	bool bWasReceivingSuccess = HeartbeatSocket->RecvFrom(Buffer.GetData(), Buffer.Num(), BytesRead, *Sender) && BytesRead > 0;
	if (!bWasReceivingSuccess)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to receive heartbeat datagram", FString(__FUNCTION__));
		return false;
	}

	// 3. Convert UTF-8 payload of known length to FString safely.
	const FUTF8ToTCHAR Converter(reinterpret_cast<const ANSICHAR*>(Buffer.GetData()), BytesRead);
	OutPayload = FString(Converter.Length(), Converter.Get());

	return true; // Successfully read a datagram and converted it to FString.
}

// Simple parser: "k=v;k2=v2;..." -> map
TMap<FString, FString> AHubGameMode::ParsePayloadToKV(const FString& Payload) const
{
	TMap<FString, FString> ServerData{};

	if (Payload.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Received empty heartbeat payload", FString(__FUNCTION__));
		return ServerData;
	}

	TArray<FString> RawData{};
	Payload.ParseIntoArray(RawData, TEXT(";"), true); // see AWorldGameMode::BuildHeartbeatPayload()

	for (const FString& Part : RawData)
	{
		FString ServerId, WorldServerView; // see FWorldServerView

		if (Part.Split(TEXT("="), &ServerId, &WorldServerView))
		{
			if (!ServerId.IsEmpty())
			{
				ServerData.Add(ServerId, WorldServerView);
			}
		}
	}

	return ServerData;
}

// Validate required keys and upsert RuntimeServers + LastSeen timestamp.
// Returns true if an entry was created/updated.
bool AHubGameMode::TryProcessHeartbeatKV(const TMap<FString, FString>& ServerData)
{
	const FString* ServerIdStr = ServerData.Find(TEXT("ServerId"));
	const FString* NameStr = ServerData.Find(TEXT("Name"));
	const FString* AddressStr = ServerData.Find(TEXT("Address"));
	if (!ServerIdStr || !NameStr || !AddressStr)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Heartbeat KV missing required keys.", FString(__FUNCTION__));
		return false;
	}

	const FName ServerId(**ServerIdStr);

	FWorldServerView& Item = RuntimeServers.FindOrAdd(ServerId);
	Item.ServerId = ServerId;
	Item.ServerName = *NameStr;
	Item.Address = *AddressStr;
	Item.CurrentPlayers = ServerData.Contains(TEXT("Current")) ? FCString::Atoi(*ServerData[TEXT("Current")]) : 0;
	Item.MaxPlayers = ServerData.Contains(TEXT("Max")) ? FCString::Atoi(*ServerData[TEXT("Max")]) : 0;
	Item.bIsOnline = true;

	LastSeenByServerId.Add(ServerId, FPlatformTime::Seconds());

	//UE_LOGFMT(LogProjectA, Log, "{0} - Updated heartbeat for ServerId={1}, Name={2}, Address={3}, Players={4}/{5}", FString(__FUNCTION__), ServerId.ToString(), Item.ServerName, Item.Address, Item.CurrentPlayers, Item.MaxPlayers);

	return true;

}

void AHubGameMode::EnterToWorldWithCharacter(APlayerController* PlayerControllerP, FName ServerId, FName CharacterId)
{
	// 1. Build a snapshot to avoid concurrent-modification issues.
	const TArray<FWorldServerView> Servers = BuildWorldServersSnapshot();

	// 2. Find the target server entry
	auto ServerIdPredicate = [ServerId](const FWorldServerView& Item) { return Item.ServerId == ServerId; };
	const FWorldServerView* Server = Servers.FindByPredicate(ServerIdPredicate);

	// 3. Validate
	if (!PlayerControllerP || !Server || !Server->bIsOnline || Server->Address.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid parameters or server is unavailable.", FString(__FUNCTION__));
		return;
	}

	const FString AccountId = PlayerAccountIds.FindRef(PlayerControllerP);
	const FString SessionToken = PlayerSessionTokens.FindRef(PlayerControllerP);

	if (AccountId.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - No AccountId found for PlayerController. Travel aborted.", FString(__FUNCTION__));
		return;
	}

	// 4. Construct a travel URL
	const FString TravelUrl = FString::Printf(
		TEXT("%s?SelectedCharacterId=%s?AccountId=%s?SessionToken=%s"),
		*Server->Address,
		*CharacterId.ToString(),
		*AccountId,
		*SessionToken
	);

	//UE_LOGFMT(LogProjectA, Log, "{0} - Travelling AccountId={1} to {2} with character={3}", FString(__FUNCTION__), *AccountId, *Server->Address, *CharacterId.ToString());

	// 5. Travel

	PlayerControllerP->ClientTravel(*TravelUrl, ETravelType::TRAVEL_Absolute);
}

void AHubGameMode::FetchAndSendCharacterListToPlayer(APlayerController* PC)
{	
	// 1. Validate
	if (!PC) { return; }

	FString AccountId, SessionToken;
	if (!TryGetPlayerAuthData(PC, AccountId, SessionToken))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: No AccountId for PlayerController. Cannot fetch characters.", FString(__FUNCTION__));
		return;
	}
	
	UCharacterService* CharacterService = GetGameInstance()->GetSubsystem<UCharacterService>();
	if (!CharacterService)
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: UCharacterService subsystem not found.", FString(__FUNCTION__));
		return;
	}

	// 2. async fetch 
	const FString TokenCopy = SessionToken;
	const FString AccountIdCopy = AccountId;

	CharacterService->FetchCharacterList(
		AccountIdCopy,
		TokenCopy,
		FOnCharacterListReceived::CreateLambda([this, PC, AccountIdCopy](const TArray<FCharacterSelectionView>& Characters)
			{
				HandleCharacterListSuccess(PC, AccountIdCopy, Characters);
			}),
		FOnCharacterServiceError::CreateLambda([this, PC](const FString& Error)
			{
				HandleCharacterListError(PC, Error);
			})
	);
}

bool AHubGameMode::TryGetPlayerAuthData(APlayerController* PC, FString& OutAccountId, FString& OutSessionToken) const
{
	const FString* AccountPtr = PlayerAccountIds.Find(PC);
	if (!AccountPtr || AccountPtr->IsEmpty())
	{
		return false;
	}

	const FString* TokenPtr = PlayerSessionTokens.Find(PC);
	OutAccountId = *AccountPtr;
	OutSessionToken = TokenPtr ? *TokenPtr : FString{};
	return true;
}

void AHubGameMode::HandleCharacterListSuccess(APlayerController* PC, const FString& AccountId, const TArray<FCharacterSelectionView>& Characters)
{
	//UE_LOGFMT(LogProjectA, Log, "{0}: Sending {1} character(s) to AccountId={2}.", FString(__FUNCTION__), Characters.Num(), *AccountId);

	ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PC);
	if (CustomPC)
	{
		CustomPC->Client_ReceiveCharacterList(Characters);
	}
}

void AHubGameMode::HandleCharacterListError(APlayerController* PC, const FString& Error)
{
	UE_LOGFMT(LogProjectA, Warning, "{0}: Failed to fetch character list: {1}", FString(__FUNCTION__), *Error);
	ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PC);
	if (CustomPC)
	{
		CustomPC->Client_ReceiveCharacterList({});
	}
}

void AHubGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		PlayerAccountIds.Remove(PC);
		PlayerSessionTokens.Remove(PC);
		UE_LOGFMT(LogProjectA, Log, "{0}: Player disconnected, session data cleared.", FString(__FUNCTION__));
	}
}
