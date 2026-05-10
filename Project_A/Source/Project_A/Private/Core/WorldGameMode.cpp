


#include "Core/WorldGameMode.h"
#include "Character/CustomPlayerController.h"

#include "Common/UdpSocketBuilder.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "TimerManager.h"
#include "GameFramework/GameState.h"
#include "ProjectNetworkSettings.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Kismet/GameplayStatics.h"

#include "ProjectALog.h"

AWorldGameMode::AWorldGameMode()
	// TODO: These default values are only for testing, they should be overridden by command line arguments or config file
	:ServerId{TEXT("MainWorld01")},	ServerName{TEXT("Main World #1")}, PublicAddress{TEXT("127.0.0.1:7778")},MaxPlayers{100}
{
	PlayerControllerClass = ACustomPlayerController::StaticClass();
}

EServerWorldType AWorldGameMode::GetMapIdentifier() const
{
	return EServerWorldType::MainWorld;
}

void AWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitialiseHeartbeat();	
}

void AWorldGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CleanupHeartbeat();

	Super::EndPlay(EndPlayReason);
}

void AWorldGameMode::InitialiseHeartbeat()
{
	FString ServerIdStr{}, ServerNameArg{}, PublicAddressArg{};
		
	bool bWas_ServerId_ObtainedFromCommandLine = FParse::Value(FCommandLine::Get(), TEXT("ServerId="), ServerIdStr) 
		&& !ServerIdStr.IsEmpty();

	bool bWas_ServerName_ObtainedFromCommandLine = FParse::Value(FCommandLine::Get(), TEXT("ServerName="), ServerNameArg) 
		&& !ServerNameArg.IsEmpty();

	bool bWas_PublicAddress_ObtainedFromCommandLine = FParse::Value(FCommandLine::Get(), TEXT("PublicAddress="), PublicAddressArg) 
		&& !PublicAddressArg.IsEmpty();

	if (bWas_ServerId_ObtainedFromCommandLine)
	{
		ServerId = FName(*ServerIdStr);
	}

	if (bWas_ServerName_ObtainedFromCommandLine)
	{
		ServerName = ServerNameArg;
	}

	if (bWas_PublicAddress_ObtainedFromCommandLine)
	{
		PublicAddress = PublicAddressArg;
	}

	// UE_LOGFMT(LogProjectA, Error, "{0} - ServerId={1}, ServerName={2}, PublicAddress={3}, MaxPlayers={4}", FString(__FUNCTION__), ServerId.ToString(), ServerName, PublicAddress, MaxPlayers);

	HeartbeatSendSocket = FUdpSocketBuilder(TEXT("WorldHeartbeatSender"))
		.AsReusable()
		.WithSendBufferSize(2 * 1024 * 1024);

	if (!HeartbeatSendSocket)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create heartbeat sender socket", FString(__FUNCTION__));
		return;
	}

	GetWorldTimerManager().SetTimer(HeartbeatSendTimerHandle, this, &AWorldGameMode::SendHeartbeat, 5.0f, true, 0.5f);
}

void AWorldGameMode::SendHeartbeat()
{
	if (!HeartbeatSendSocket)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - HeartbeatSendSocket is not initialized", FString(__FUNCTION__));
		return;
	}
	/*UE_LOGFMT(LogProjectA, Log, "{0} - Heartbeat: Id={1}, Name={2}, Addr={3}", FString(__FUNCTION__), ServerId.ToString(), ServerName, PublicAddress);*/


	// 1. Network preparations

	FIPv4Address HubIp{};
	const bool bParseResult = FIPv4Address::Parse(NetSet::HubHeartbeatAddress, HubIp);

	if (!bParseResult)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid HubHeartbeatAddress: {1}", FString(__FUNCTION__), FString(NetSet::HubHeartbeatAddress));
		return;
	}
	
	const FString Payload = BuildHeartbeatPayload();
	TSharedRef<FInternetAddr> HubAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	HubAddr->SetIp(HubIp.Value);
	HubAddr->SetPort(NetSet::HubHeartbeatPort);

	FTCHARToUTF8 Converter(*Payload);
	int32 BytesSent = 0;

	// 2. Send heartbeat
	HeartbeatSendSocket->SendTo(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length(), BytesSent, *HubAddr);

	UE_LOGFMT(LogProjectA, Log, "{0} - Heartbeat sent, BytesSent={1}", FString(__FUNCTION__), BytesSent);
}



void AWorldGameMode::CleanupHeartbeat()
{
	GetWorldTimerManager().ClearTimer(HeartbeatSendTimerHandle);

	if (HeartbeatSendSocket)
	{
		HeartbeatSendSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(HeartbeatSendSocket);
		HeartbeatSendSocket = nullptr;
	}
}

FString AWorldGameMode::BuildHeartbeatPayload() const
{
	const int32 CurrentPlayers = GetPlayerCount();

	/* 1 = true */ 
	/* see AHubGameMode::ParsePayloadToKV */

	return FString::Printf(TEXT("ServerId=%s;Name=%s;Address=%s;Current=%d;Max=%d;Online=1"), 
		*ServerId.ToString(),
		*ServerName,
		*PublicAddress,
		CurrentPlayers,
		MaxPlayers);
}

int32 AWorldGameMode::GetPlayerCount() const
{
	int32 PlayerCount{0};
	const AGameStateBase* GameStateP = GetGameState<AGameStateBase>();

	if (GameStateP)
	{
		PlayerCount = GameStateP->PlayerArray.Num();
	}

	return PlayerCount;
}

FString AWorldGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	// 1. Validation
	const FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	if (!ErrorMessage.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Super::InitNewPlayer failed with error: {1}", FString(__FUNCTION__), ErrorMessage);
		return ErrorMessage;
	}

	ACustomPlayerController* CustomPlayerController = Cast<ACustomPlayerController>(NewPlayerController);
	if (!CustomPlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - NewPlayerController is not of type ACustomPlayerController", FString(__FUNCTION__));
		return ErrorMessage;
	}

	// 2. Get data from URL options
	const FString CharacterIdStr = UGameplayStatics::ParseOption(Options, TEXT("SelectedCharacterId"));
	const FName CharacterId = CharacterIdStr.IsEmpty() ? NAME_None : FName(*CharacterIdStr);

	const FCharacterSpawnDefinition* Definition = CharacterDefinitionById.Find(CharacterId);
	if (!Definition || !Definition->PawnClass)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid SelectedCharacterId={1}", FString(__FUNCTION__), CharacterId.ToString());
		return TEXT("Invalid character selection");
	}

	//3. Store selected character data in PlayerController for later use in GetDefaultPawnClassForController
	CustomPlayerController->SetSelectedCharacterId(CharacterId);
	CustomPlayerController->SetSelectedCharacterLevel(Definition->Level);

	//UE_LOGFMT(LogProjectA, Log, "{0} - Character resolved: Id={1}, Level={2}", FString(__FUNCTION__), CharacterId.ToString(), Definition->Level);

	return ErrorMessage; // Return empty string to indicate success
}

UClass* AWorldGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	//1. Validation
	const ACustomPlayerController* CustomPlayerController = Cast<ACustomPlayerController>(InController);
	if (!CustomPlayerController)
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	const FName CharacterId = CustomPlayerController->GetSelectedCharacterId();
	const FCharacterSpawnDefinition* Definition = CharacterDefinitionById.Find(CharacterId);
	if (!Definition || !Definition->PawnClass)
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}
	
	//2. Return Pawn class based on the character selected by a player in the character selection screen on the Hub-server.
	return Definition->PawnClass.Get();
}
