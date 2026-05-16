#include "Character/LobbyPlayerController.h"

#include "Core/LobbyGameMode.h"
#include "Core/LobbyGameState.h"
#include "Core/LobbyPlayerState.h"
#include "LobbyHud.h"
#include "HAL/PlatformTime.h"

#include "ProjectALog.h"


void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindToLobbyGameState();
	PushLobbyStateToHud();
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromLobbyGameState();

	Super::EndPlay(EndPlayReason);
}

void ALobbyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PushLobbyStateToHud();
}

void ALobbyPlayerController::HandleLobbyStateChanged()
{
	PushLobbyStateToHud();
}

void ALobbyPlayerController::BindToLobbyGameState()
{
	ALobbyGameState* LobbyGameState = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
	if (!LobbyGameState)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	if (CachedLobbyGameState.Get() == LobbyGameState) { return; }

	UnbindFromLobbyGameState();

	CachedLobbyGameState = LobbyGameState;
	LobbyGameState->OnLobbyStateChangedNative.AddUObject(this, &ALobbyPlayerController::HandleLobbyStateChanged);
}

void ALobbyPlayerController::SetReadyStatus(bool bNewIsReady)
{	
	Server_SetReadyStatus(bNewIsReady);
}

void ALobbyPlayerController::ForceStartMatch()
{
	Server_ForceStartMatch();
}

void ALobbyPlayerController::Server_SetReadyStatus_Implementation(bool bNewIsReady)
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (!LobbyGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	LobbyGameMode->SetReadyForPlayer(this, bNewIsReady);
}

void ALobbyPlayerController::Server_ForceStartMatch_Implementation()
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (!LobbyGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	LobbyGameMode->RequestForceStart(this);
}

void ALobbyPlayerController::UnbindFromLobbyGameState()
{
	if (!CachedLobbyGameState.IsValid())
	{
		return;
	}

	CachedLobbyGameState->OnLobbyStateChangedNative.RemoveAll(this);
	CachedLobbyGameState = nullptr;
}

void ALobbyPlayerController::SetSelectedCharacterId(FName CharacterId)
{
	Server_SetSelectedCharacterId(CharacterId);
}

void ALobbyPlayerController::Server_SetSelectedCharacterId_Implementation(FName CharacterId)
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (!LobbyGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	LobbyGameMode->SetSelectedCharacterForPlayer(this, CharacterId);
}

void ALobbyPlayerController::CreateLobby()
{
	Server_CreateLobby();
}

void ALobbyPlayerController::JoinLobby(FName LobbyCode)
{
	Server_JoinLobby(LobbyCode);
}

void ALobbyPlayerController::LeaveLobby()
{
	CleanupCachedRoomState();

	PushLobbyStateToHud();

	Server_LeaveLobby();
}

void ALobbyPlayerController::CleanupCachedRoomState()
{
	CachedClientLobbyCode = NAME_None;
	LastRoomRpcTime = FPlatformTime::Seconds();

	bHasCachedRoomState = false;
	CachedRoomLobbyPlayers.Reset();
	CachedRoomAvailableCharacterIds.Reset();
	CachedRoomSelectedCharacterId = NAME_None;
	bCachedRoomMatchStarting = false;
	CachedRoomCountdownSeconds = -1;
	bCachedRoomLocalReady = false;
}

void ALobbyPlayerController::Server_CreateLobby_Implementation()
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (!LobbyGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	LobbyGameMode->CreateRoomForPlayer(this);
}

void ALobbyPlayerController::Server_JoinLobby_Implementation(FName LobbyCode)
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (!LobbyGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	LobbyGameMode->JoinRoomForPlayer(this, LobbyCode);	
}

void ALobbyPlayerController::Server_LeaveLobby_Implementation()
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (!LobbyGameMode)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyGameMode is nullptr", FString(__FUNCTION__));
		return;
	}

	LobbyGameMode->LeaveRoomForPlayer(this);	
}

void ALobbyPlayerController::PushLobbyStateToHud()
{
	ALobbyHud* LobbyHud = Cast<ALobbyHud>(GetHUD());
	const ALobbyGameState* LobbyGameState = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
	const ALobbyPlayerState* LobbyPlayerState = GetPlayerState<ALobbyPlayerState>();

	if (!LobbyHud || !LobbyGameState)
	{
		//UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyHud or LobbyGameState is nullptr", FString(__FUNCTION__));
		return;
	}

	const FName LobbyCodeToUse = ResolveLobbyCodeToUse(LobbyPlayerState);

	// If should show room RPC snapshot — use cached snapshot, otherwise fallback to replicated GameState + PlayerState.
	if (!LobbyCodeToUse.IsNone() && bHasCachedRoomState && CachedClientLobbyCode == LobbyCodeToUse)
	{
		PushRoomCacheToHud(LobbyHud, LobbyCodeToUse);
	}
	else
	{
		PushGameStateToHud(LobbyHud, LobbyCodeToUse, LobbyGameState, LobbyPlayerState);
	}

	// Rooms (browser UI) always come from GameState replication
	LobbyHud->UpdateMainWidget(LobbyGameState->GetLobbyRooms());
}

FName ALobbyPlayerController::ResolveLobbyCodeToUse(const ALobbyPlayerState* LobbyPlayerState) const
{
	const double Now = FPlatformTime::Seconds();
	const double GraceSeconds = 0.5;

	const FName PlayerStateLobbyCode = LobbyPlayerState ? LobbyPlayerState->GetLobbyCode() : NAME_None;

	// during short window after Client_UpdateLobbyRoomState we trust RPC cache
	if ((Now - LastRoomRpcTime) < GraceSeconds)
	{
		return CachedClientLobbyCode;
	}

	// otherwise prefer PlayerState (replicated)
	if (!PlayerStateLobbyCode.IsNone())
	{
		return PlayerStateLobbyCode;
	}

	return NAME_None;
}

int32 ALobbyPlayerController::CountRoomReadyPlayers() const
{
	int32 RoomReadyPlayers = 0;
	for (const FLobbyPlayerView& Item : CachedRoomLobbyPlayers)
	{
		if (Item.bIsReady)
		{
			++RoomReadyPlayers;
		}
	}
	return RoomReadyPlayers;
}

void ALobbyPlayerController::PushRoomCacheToHud(ALobbyHud* LobbyHud, FName LobbyCodeToUse) const
{
	// Use RPC-cached local ready (bCachedRoomLocalReady) when showing room RPC state.
	LobbyHud->UpdateRoomState(
		LobbyCodeToUse,
		CachedRoomLobbyPlayers.Num(),
		CountRoomReadyPlayers(),
		bCachedRoomMatchStarting,
		CachedRoomCountdownSeconds,
		bCachedRoomLocalReady,
		CachedRoomAvailableCharacterIds,
		CachedRoomSelectedCharacterId,
		CachedRoomLobbyPlayers
	);
}

void ALobbyPlayerController::PushGameStateToHud(ALobbyHud* LobbyHud, FName LobbyCodeToUse, const ALobbyGameState* LobbyGameState, const ALobbyPlayerState* LobbyPlayerState) const
{
	const bool bLocalReadyFromPlayerState = LobbyPlayerState ? LobbyPlayerState->IsReadyStatus() : false;
	const FName SelectedCharacterIdFromPlayerState = LobbyPlayerState ? LobbyPlayerState->GetSelectedCharacterId() : NAME_None;

	LobbyHud->UpdateRoomState(
		LobbyCodeToUse,
		LobbyGameState->GetConnectedPlayers(),
		LobbyGameState->GetReadyPlayers(),
		LobbyGameState->IsMatchStarting(),
		LobbyGameState->GetCountdownSeconds(),
		bLocalReadyFromPlayerState,
		LobbyGameState->GetAvailableCharacterIds(),
		SelectedCharacterIdFromPlayerState,
		LobbyGameState->GetLobbyPlayers()
	);
}

void ALobbyPlayerController::Client_UpdateLobbyRoomState_Implementation(FName LobbyCode, const TArray<FLobbyPlayerView>& LobbyPlayers,
	bool bMatchStarting, int32 CountdownSeconds, bool bLocalReady, const TArray<FName>& AvailableCharacterIds, 
	FName SelectedCharacterId)
{
	CachedClientLobbyCode = LobbyCode;
	LastRoomRpcTime = FPlatformTime::Seconds();

	bHasCachedRoomState = !LobbyCode.IsNone();
	CachedRoomLobbyPlayers = LobbyPlayers;
	bCachedRoomMatchStarting = bMatchStarting;
	CachedRoomCountdownSeconds = CountdownSeconds;
	CachedRoomAvailableCharacterIds = AvailableCharacterIds;
	CachedRoomSelectedCharacterId = SelectedCharacterId;
	bCachedRoomLocalReady = bLocalReady;

	if (LobbyCode.IsNone())
	{
		CleanupCachedRoomState();
	}

	PushLobbyStateToHud();
}
