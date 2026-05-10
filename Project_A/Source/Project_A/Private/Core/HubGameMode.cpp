
#include "Core/HubGameMode.h"
#include "Character/CustomPlayerController.h"
#include "ProjectNetworkSettings.h"
#include "Common/UdpSocketBuilder.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "TimerManager.h"
#include "HAL/PlatformTime.h"


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
		.BoundToPort(NetSet::HubHeartbeatPort)
		.WithReceiveBufferSize(2 * 1024 * 1024);

	if (!HeartbeatSocket)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to bind heartbeat socket on port {1}", FString(__FUNCTION__), NetSet::HubHeartbeatPort);
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

	//UE_LOGFMT(LogProjectA, Log, "{0} - Updated heartbeat for ServerId={1}, Name={2}, Address={3}, Players={4}/{5}",
	//	FString(__FUNCTION__), ServerId.ToString(), Item.ServerName, Item.Address, Item.CurrentPlayers, Item.MaxPlayers);

	return true;

}


TArray<FCharacterSelectionView> AHubGameMode::BuildMockCharactersForServer(FName ServerId) const
{
	// Simple temporary solution
	// @see ACustomPlayerController::Server_RequestCharacterList_Implementation

	TArray<FCharacterSelectionView> Result{};

	FCharacterSelectionView C1{};
	C1.CharacterId = FName(TEXT("Warrior_01"));
	C1.CharacterName = TEXT("Ragnar");
	C1.Level = 12;
	C1.ClassName = TEXT("Warrior");

	FCharacterSelectionView C2{};
	C2.CharacterId = FName(TEXT("Mage_01"));
	C2.CharacterName = TEXT("Arlen");
	C2.Level = 9;
	C2.ClassName = TEXT("Mage");

	Result.Add(C1);
	Result.Add(C2);

	return Result;
}

void AHubGameMode::EnterToWorldWithCharacter(APlayerController* PlayerControllerP, FName ServerId, FName CharacterId)
{
	const TArray<FWorldServerView> Servers = BuildWorldServersSnapshot();

	auto ServerIdPredicate = [ServerId](const FWorldServerView& Item) { return Item.ServerId == ServerId; };
	const FWorldServerView* Server = Servers.FindByPredicate(ServerIdPredicate);

	if (!PlayerControllerP || !Server || !Server->bIsOnline || Server->Address.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid parameters or server is unavailable.", FString(__FUNCTION__));
		return;
	}

	// SelectedCharacterId — a simple way to pass the selected character info to the world server. 
	const FString TravelUrl = FString::Printf(TEXT("%s?SelectedCharacterId=%s"), *Server->Address, *CharacterId.ToString());

	PlayerControllerP->ClientTravel(*TravelUrl, ETravelType::TRAVEL_Absolute);
}


//void AHubGameMode::EnterToWorld(APlayerController* PlayerControllerP, FName ServerId)
//{
//	if (!PlayerControllerP)
//	{
//		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is empty", FString(__FUNCTION__));   
//		return;
//	}
//
//	/*PlayerControllerP->ClientTravel(NetSet::MainWorldServerAddress, ETravelType::TRAVEL_Absolute);*/
//
//	const TArray<FWorldServerView> Servers = BuildWorldServersSnapshot();
//	const FWorldServerView* Server = Servers.FindByPredicate(
//		[ServerId](const FWorldServerView& Item)
//		{
//			return Item.ServerId == ServerId;
//		});
//
//	if (!Server)
//	{
//		UE_LOGFMT(LogProjectA, Warning, "{0} - ServerId not found: {1}", FString(__FUNCTION__), ServerId.ToString());
//		return;
//	}
//
//	if (!Server->bIsOnline || Server->Address.IsEmpty())
//	{
//		UE_LOGFMT(LogProjectA, Warning, "{0} - Server is unavailable: {1}", FString(__FUNCTION__), ServerId.ToString());
//		return;
//	}
//
//	PlayerControllerP->ClientTravel(*Server->Address, ETravelType::TRAVEL_Absolute);
//}