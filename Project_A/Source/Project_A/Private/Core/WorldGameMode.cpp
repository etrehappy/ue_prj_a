


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

#include "NetPlayerState.h"
#include "QuestManagerSubsystem.h"
#include "QuestDefinition.h"

#include "CharacterService.h"
#include "Core/GameInstanceBase.h"
#include "HealthComponent.h"

#include "ProjectALog.h"

AWorldGameMode::AWorldGameMode()
	// These default values are only for testing, they should be overridden by command line arguments or config file
	:ServerId{TEXT("MainWorld01")},	ServerName{TEXT("Main World #1")}, PublicAddress{TEXT("127.0.0.1:7778")}, MaxPlayers{100}
{
	PlayerControllerClass = ACustomPlayerController::StaticClass();
	PlayerStateClass = ANetPlayerState::StaticClass();
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
	const bool bParseResult = FIPv4Address::Parse(HubHeartbeatAddress, HubIp);

	if (!bParseResult)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid HubHeartbeatAddress: {1}", FString(__FUNCTION__), FString(HubHeartbeatAddress));
		return;
	}
	
	const FString Payload = BuildHeartbeatPayload();
	TSharedRef<FInternetAddr> HubAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	HubAddr->SetIp(HubIp.Value);
	HubAddr->SetPort(HubHeartbeatPort);

	FTCHARToUTF8 Converter(*Payload);
	int32 BytesSent = 0;

	// 2. Send heartbeat
	HeartbeatSendSocket->SendTo(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length(), BytesSent, *HubAddr);

	//UE_LOGFMT(LogProjectA, Log, "{0} - Heartbeat sent, BytesSent={1}", FString(__FUNCTION__), BytesSent);
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

FString AWorldGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if (!Result.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: Super::InitNewPlayer failed: {1}", FString(__FUNCTION__), *Result);
		return Result;
	}

	// 1. Parsing
	const FString CharacterIdStr = UGameplayStatics::ParseOption(Options, TEXT("SelectedCharacterId"));
	const FName SelectedCharacterId = CharacterIdStr.IsEmpty() ? NAME_None : FName(*CharacterIdStr);

	FPlayerSessionData SessionData;
	SessionData.AccountId = UGameplayStatics::ParseOption(Options, TEXT("AccountId"));
	SessionData.SessionToken = UGameplayStatics::ParseOption(Options, TEXT("SessionToken"));
	SessionData.CharacterId = SelectedCharacterId;

	// 2. Keep data
	PlayerSessionDataMap.Add(NewPlayerController, SessionData);
		
	if (ANetPlayerState* PS = NewPlayerController ? NewPlayerController->GetPlayerState<ANetPlayerState>() : nullptr)
	{
		PS->SetAccountId(SessionData.AccountId);
		PS->SetSessionToken(SessionData.SessionToken);
		PS->SetCharacterId(SessionData.CharacterId);
	}

	if (SessionData.AccountId.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: Player connected without AccountId. Options={1}", FString(__FUNCTION__), *Options);
	}

	// 3. Logging

	return Result;
}
void AWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer)
	{
		return;
	}

	LoadAndApplyCharacterData(NewPlayer);
}

void AWorldGameMode::Logout(AController* Exiting)
{
	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		SaveCharacterFromPawn(PC);
		PlayerSessionDataMap.Remove(PC);
		UE_LOGFMT(LogProjectA, Log, "{0}: Player disconnected, session data cleared.", FString(__FUNCTION__));
	}

	Super::Logout(Exiting);
}

void AWorldGameMode::LoadAndApplyCharacterData(APlayerController* PC)
{
	// 1. Validate and extract
	FString AccountId;
	FString SessionToken;
	FName CharacterId;
	if (!TryGetSessionForPlayer(PC, AccountId, SessionToken, CharacterId))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: Missing session data for PC — skipping character load.", FString(__FUNCTION__));
		return;
	}
	UCharacterService* CharacterService = GetGameInstance()->GetSubsystem<UCharacterService>();
	if (!CharacterService)
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: UCharacterService subsystem not found.", FString(__FUNCTION__));
		return;
	}

	// 2. async load
	CharacterService->LoadCharacter(
		AccountId,
		CharacterId,
		SessionToken,
		FOnCharacterLoaded::CreateLambda([this, PC, AccountId, CharacterId](const FCharacterSaveData& SaveData)
			{
				HandleCharacterLoaded(PC, AccountId, CharacterId, SaveData);
			}),
		FOnCharacterServiceError::CreateLambda([this, AccountId, CharacterId](const FString& Error)
			{
				HandleCharacterLoadError(AccountId, CharacterId, Error);
			})
	);
}

bool AWorldGameMode::TryGetSessionForPlayer(APlayerController* PC, FString& OutAccountId, FString& OutSessionToken, FName& OutCharacterId) const
{
	const FPlayerSessionData* SessionData = GetSessionData(PC);
	if (!SessionData || SessionData->AccountId.IsEmpty() || SessionData->CharacterId.IsNone())
	{
		return false;
	}

	OutAccountId = SessionData->AccountId;
	OutSessionToken = SessionData->SessionToken;
	OutCharacterId = SessionData->CharacterId;
	return true;
}

void AWorldGameMode::HandleCharacterLoaded(APlayerController* PC, const FString& AccountId, FName CharacterId, const FCharacterSaveData& SaveData)
{
	// UE_LOGFMT(LogProjectA, Log, "{0}: Loaded character {1} for AccountId={2}.", FString(__FUNCTION__), *CharacterId.ToString(), *AccountId);

	// 1. Update fields
	if (PC)
	{
		if (ANetPlayerState* PS = PC->GetPlayerState<ANetPlayerState>())
		{
			PS->SetCharacterName(SaveData.CharacterName);
			PS->SetLevel(SaveData.Level);
			PS->SetClassName(SaveData.ClassName);
		}
	}

	// 2. Apply data to pawn if ready, otherwise retry once after a short delay
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (Pawn)
	{
		ApplySaveDataToPawn(Pawn, SaveData);
		return;
	}

	// 3. Pawn not yet spawned — retry once after a short delay.
	FTimerHandle RetryHandle;
	const FCharacterSaveData SaveCopy = SaveData;	
	TWeakObjectPtr<APlayerController> WeakPC(PC);
		
	FTimerDelegate RetryDelegate = FTimerDelegate::CreateLambda([this, WeakPC, SaveCopy]()
		{
			APlayerController* RetryPC = WeakPC.Get();
			if (!RetryPC)
			{
				// PlayerController no longer valid — nothing to do.
				return;
			}

			APawn* RetryPawn = RetryPC->GetPawn();
			if (RetryPawn)
			{
				ApplySaveDataToPawn(RetryPawn, SaveCopy);
			}
		});

	// Schedule a single retry after 0.5 seconds.
	GetWorldTimerManager().SetTimer(RetryHandle, RetryDelegate, 0.5f, false);
}

void AWorldGameMode::HandleCharacterLoadError(const FString& AccountId, FName CharacterId, const FString& Error)
{
	UE_LOGFMT(LogProjectA, Warning, "{0}: Failed to load character {1} for AccountId={2}. Error={3}. Using defaults.", FString(__FUNCTION__), *CharacterId.ToString(), *AccountId, *Error);
	// Fallback: spawn defaults (no further action needed here)
}
void AWorldGameMode::ApplySaveDataToPawn(APawn* InPawn, const FCharacterSaveData& SaveData)
{
	if (!InPawn || !SaveData.IsValid())	{ return; }

	// Restore last known position
	if (!SaveData.LastPosition.IsZero())
	{
		InPawn->SetActorLocationAndRotation(SaveData.LastPosition, SaveData.LastRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	/*UE_LOGFMT(LogProjectA, Log, "{0}: Applied save data to pawn. Position={1} Health={2}/{3}",
		FString(__FUNCTION__),
		*SaveData.LastPosition.ToString(),
		SaveData.CurrentHealth,
		SaveData.MaxHealth);*/
}


void AWorldGameMode::SaveCharacterFromPawn(APlayerController* PC)
{
	// 1. Validation

	if (!PC)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: Invalid PlayerController, cannot save character.", FString(__FUNCTION__));
		return;
	}

	const FPlayerSessionData* SessionData = GetSessionData(PC);
	if (!SessionData || SessionData->AccountId.IsEmpty() || SessionData->CharacterId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: Missing session data for PlayerController={1}, cannot save character.",
			FString(__FUNCTION__), *GetNameSafe(PC));
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0}: PlayerController={1} has no pawn, cannot save character.",
			FString(__FUNCTION__), *GetNameSafe(PC));
		return;
	}

	UCharacterService* CharacterService = GetGameInstance()->GetSubsystem<UCharacterService>();
	if (!CharacterService)
	{
		UE_LOGFMT(LogProjectA, Error, "{0}: UCharacterService subsystem not found, cannot save character.", FString(__FUNCTION__));
		return;
	}

	// 2. Build save snapshot from current pawn state
	FCharacterSaveData SaveData;
	SaveData.CharacterId = SessionData->CharacterId;
	SaveData.LastPosition = Pawn->GetActorLocation();
	SaveData.LastRotation = Pawn->GetActorRotation();
	SaveData.LastMapId = FName(*GetWorld()->GetMapName());

	if (ANetPlayerState* PS = PC->GetPlayerState<ANetPlayerState>())
	{
		SaveData.CharacterName = PS->GetCharacterName();
		SaveData.Level = PS->GetLevel();
		SaveData.ClassName = PS->GetClassName();
	}

	const FString AccountId = SessionData->AccountId;
	const FString SessionToken = SessionData->SessionToken;

	// 3. async save
	CharacterService->SaveCharacter(
		AccountId,
		SessionToken,
		SaveData,
		FOnCharacterSaved::CreateLambda([AccountId, CharacterId = SessionData->CharacterId]()
			{
				UE_LOGFMT(LogProjectA, Log, "{0}: Character {1} saved for AccountId={2}.",
					FString(__FUNCTION__), *CharacterId.ToString(), *AccountId);
			}),
		FOnCharacterServiceError::CreateLambda([AccountId](const FString& Error)
			{
				UE_LOGFMT(LogProjectA, Warning, "{0}: Failed to save character for AccountId={1}. Error={2}",
					FString(__FUNCTION__), *AccountId, *Error);
			})
	);
}

const FPlayerSessionData* AWorldGameMode::GetSessionData(APlayerController* PC) const
{
	return PlayerSessionDataMap.Find(PC);
}

UClass* AWorldGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	APlayerController* PC = Cast<APlayerController>(InController);
	if (!PC)
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	const FPlayerSessionData* SessionData = GetSessionData(PC);
	if (!SessionData || SessionData->CharacterId.IsNone())
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	const FCharacterSpawnDefinition* Definition = CharacterDefinitionById.Find(SessionData->CharacterId);
	if (!Definition || !Definition->PawnClass)
	{
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	return Definition->PawnClass.Get();
}

void AWorldGameMode::TryJoinParty(APawn* RequesterPawn, APawn* TargetPawn)
{
	// 1. Validation
	if (!RequesterPawn || !TargetPawn || RequesterPawn == TargetPawn)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid RequesterPawn or TargetPawn", FString(__FUNCTION__));
		return;
	}

	ANetPlayerState* RequesterPlayerState = RequesterPawn->GetPlayerState<ANetPlayerState>();
	ANetPlayerState* TargetPlayerState = TargetPawn->GetPlayerState<ANetPlayerState>();
	if (!RequesterPlayerState || !TargetPlayerState)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - RequesterPawn or TargetPawn does not have a valid ANetPlayerState", FString(__FUNCTION__));
		return;
	}

	// 2. Preparation
	const FName RequesterPartyId = RequesterPlayerState->GetPartyId();
	const FName TargetPartyId = TargetPlayerState->GetPartyId();

	FName ResolvedPartyId{NAME_None};

	bool bRequestPlayerInParty = !RequesterPartyId.IsNone();
	bool bTargetPlayerInParty = !TargetPartyId.IsNone();
	bool bBothPlayersNotInParty = RequesterPartyId.IsNone() && TargetPartyId.IsNone();

	// 3. Party logic
	if (bBothPlayersNotInParty)
	{
		ResolvedPartyId = FName(*FString::Printf(TEXT("Party_%d"), NextPartyNumericId++));
		RequesterPlayerState->SetPartyId(ResolvedPartyId);
		TargetPlayerState->SetPartyId(ResolvedPartyId);
	}
	else if (bRequestPlayerInParty && !bTargetPlayerInParty)
	{
		ResolvedPartyId = RequesterPartyId;
		TargetPlayerState->SetPartyId(ResolvedPartyId);
	}
	else if (!bRequestPlayerInParty && bTargetPlayerInParty)
	{
		ResolvedPartyId = TargetPartyId;
		RequesterPlayerState->SetPartyId(ResolvedPartyId);
	}
	else // both in partys
	{		
		if (RequesterPartyId == TargetPartyId)
		{
			ResolvedPartyId = RequesterPartyId;
		}
		else
		{
			return;
		}
	}

	PushPartyMembersToClients(ResolvedPartyId);
}



void AWorldGameMode::PushPartyMembersToClients(FName PartyId)
{
	if (!GameState)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - GameState is not valid", FString(__FUNCTION__));
		return;
	}

	if(PartyId.IsNone())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PartyId is not valid", FString(__FUNCTION__));
		return;
	}

	TArray<APawn*> PartyPawns{};
	TArray<ACustomPlayerController*> PartyControllers{};

	for (APlayerState* PlayerStateBase : GameState->PlayerArray) // TODO: it can be optimized by keeping track of parties in the GameMode and updating them on player join/leave instead of iterating through all players in the GameState every time
	{
		ANetPlayerState* Ps = Cast<ANetPlayerState>(PlayerStateBase);
		if (!Ps || Ps->GetPartyId() != PartyId)
		{
			continue;
		}

		AController* OwnerController = Cast<AController>(Ps->GetOwner());
		ACustomPlayerController* Pc = Cast<ACustomPlayerController>(OwnerController);
		if (!Pc)
		{
			continue;
		}

		if (APawn* Pawn = Pc->GetPawn())
		{
			PartyPawns.Add(Pawn);
		}

		PartyControllers.Add(Pc);
	}

	for (ACustomPlayerController* Pc : PartyControllers)
	{
		Pc->Client_UpdatePartyMembers(PartyPawns);
	}
}

