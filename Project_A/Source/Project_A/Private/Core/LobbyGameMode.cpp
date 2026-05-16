#include "Core/LobbyGameMode.h"

#include "Character/LobbyPlayerController.h"
#include "Core/LobbyGameState.h"
#include "Core/LobbyPlayerState.h"
#include "LobbyPlayerView.h"
#include "LobbyHud.h"
#include "ProjectNetworkSettings.h"

#include "ProjectALog.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	PlayerStateClass = ALobbyPlayerState::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();
	HUDClass = ALobbyHud::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	HandleLobbyStateChanged();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(Exiting))
	{
		LeaveRoomForPlayer(LobbyPlayerController);
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Exiting controller is not a LobbyPlayerController", FString(__FUNCTION__));
	}

	Super::Logout(Exiting);
}

void ALobbyGameMode::StartMatchTravelForRoom(FName RoomCode)
{
	// 1. Validation
	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — No room found for code {1}", FString(__FUNCTION__), *RoomCode.ToString());
		return;
	}
		
	if (LobbyTargetWorldAddress.IsEmpty())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — No target world address set (LobbyTargetWorldAddress is empty)", FString(__FUNCTION__));
		return;
	}

	for (const TWeakObjectPtr<ALobbyPlayerController>& PlayerControllerPtr : Room->Players)
	{
		ALobbyPlayerController* PlayerController = PlayerControllerPtr.Get();
		if (!PlayerController) { continue; }

		ALobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<ALobbyPlayerState>();
		if (!LobbyPlayerState) { continue; }

		const FName SelectedCharacterId = LobbyPlayerState->GetSelectedCharacterId();
		if (SelectedCharacterId.IsNone()) { continue; }

	// 2. Preparation
		const FString TravelUrl = FString::Printf(TEXT("%s?SelectedCharacterId=%s"),
			*LobbyTargetWorldAddress, *SelectedCharacterId.ToString()
		);

	// 3. Action
		PlayerController->ClientTravel(TravelUrl, ETravelType::TRAVEL_Absolute);
	}
}

void ALobbyGameMode::CreateRoomForPlayer(ALobbyPlayerController* Requester)
{
	if (!Requester)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Requester is null", FString(__FUNCTION__));
		return;
	}

	LeaveRoomForPlayer(Requester); // Ensure the player leaves any existing lobby before creating a new one

	const FName RoomCode = GenerateLobbyCode();
	if (RoomCode.IsNone())
	{	
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to generate a unique lobby code", FString(__FUNCTION__));
		return;
	}

	if (!SetRoomConfig(Requester, RoomCode))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to set lobby config for room code {1}", FString(__FUNCTION__), *RoomCode.ToString());
		return;
	}
	
	ALobbyPlayerState* LobbyPlayerState = Requester->GetPlayerState<ALobbyPlayerState>();
	EName DefaultCharacterData = NAME_None;

	if (LobbyPlayerState && !LobbyPlayerState->InisialiseLobbyPlayerState(false, DefaultCharacterData, RoomCode))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to initialize lobby player state for requester", FString(__FUNCTION__));
		return;
	}
		
	HandleLobbyStateChanged();
}

void ALobbyGameMode::JoinRoomForPlayer(ALobbyPlayerController* Requester, FName RoomCode)
{
	// 1. Preparation and validation
	if (!Requester || RoomCode.IsNone()) 
	{ 
		UE_LOGFMT(LogProjectA, Log, "{0} — Invalid requester or room code", FString(__FUNCTION__));
		return; 
	}

	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room) 
	{ 
		UE_LOGFMT(LogProjectA, Log, "{0} — No room found for code {1}", FString(__FUNCTION__), *RoomCode.ToString());
		return; 
	}

	LeaveRoomForPlayer(Requester);

	// 2. Update the room
	Room->Players.AddUnique(Requester);
	RoomCodeByPlayer.Add(Requester, RoomCode);

	ALobbyPlayerState* LobbyPlayerState = Requester->GetPlayerState<ALobbyPlayerState>();
	EName DefaultCharacterData = NAME_None;

	if (LobbyPlayerState && !LobbyPlayerState->InisialiseLobbyPlayerState(false, DefaultCharacterData, RoomCode))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to initialize lobby player state for requester", FString(__FUNCTION__));
		return;
	}
	
	HandleLobbyStateChanged();
}

void ALobbyGameMode::LeaveRoomForPlayer(ALobbyPlayerController* Requester)
{

	// 1. Try fetch room code + runtime entry for requester
	FName OldRoomCode = NAME_None;
	FLobbyRoomRuntime* Room = nullptr;
	if (!TryFetchPlayerRoom(Requester, OldRoomCode, Room))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} — Requester is not in any room", FString(__FUNCTION__));
		return;
	}

	// 2. If room entry missing on server side — just clean client map and notify requester
	if (!Room)
	{
		RoomCodeByPlayer.Remove(Requester);
		SendClientClearedRoomState(Requester);
		HandleLobbyStateChanged();
		return;
	}

	// 3. Remove player from room containers and reset player state
	RemovePlayerReferencesFromRoom(Room, Requester);
	ResetPlayerLobbyState(Requester);

	// 4. If room became empty — cleanup room
	MaybeDestroyRoomIfEmpty(OldRoomCode, Room);

	// 5. Notify requester and refresh global state
	SendClientClearedRoomState(Requester);
	HandleLobbyStateChanged();
}

void ALobbyGameMode::SetReadyForPlayer(ALobbyPlayerController* Requester, bool bNewReady)
{
	FName RoomCode = NAME_None;
	if (!TryGetPlayerRoomCode(Requester, RoomCode))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} — Requester is not in any room", FString(__FUNCTION__));
		return;
	}

	ALobbyPlayerState* LobbyPlayerState = Requester->GetPlayerState<ALobbyPlayerState>();
	if (!LobbyPlayerState)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to get player state for requester", FString(__FUNCTION__));
		return;
	}

	LobbyPlayerState->SetReadyStatus(bNewReady);
	HandleLobbyStateChanged();
}

void ALobbyGameMode::SetSelectedCharacterForPlayer(ALobbyPlayerController* Requester, FName CharacterId)
{
	// 1. Validation
	if (!Requester)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Invalid requester", FString(__FUNCTION__));
		return;
	}

	// CharacterId = None is allowed (player cancels character selection).
	// Validate only non-empty character IDs.
	if (!CharacterId.IsNone() && !IsCharacterAllowed(CharacterId))
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Character ID is not allowed: {1}", FString(__FUNCTION__), *CharacterId.ToString());
		return;
	}

	FName RoomCode = NAME_None;
	if (!TryGetPlayerRoomCode(Requester, RoomCode))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} — Requester is not in any room", FString(__FUNCTION__));
		return;
	}

	ALobbyPlayerState* LobbyPlayerState = Requester->GetPlayerState<ALobbyPlayerState>();
	if (!LobbyPlayerState)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to get player state for requester", FString(__FUNCTION__));
		return;
	}

	// 2. Apply selection (or deselection)
	LobbyPlayerState->SetSelectedCharacterId(CharacterId);

	// If selection is canceled, player cannot stay ready.
	if (CharacterId.IsNone())
	{
		LobbyPlayerState->SetReadyStatus(false);
	}

	CancelCountdownForRoom(RoomCode);
	HandleLobbyStateChanged();
}
void ALobbyGameMode::RequestForceStart(ALobbyPlayerController* Requester)
{
	if (!Requester)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Invalid requester", FString(__FUNCTION__));
		return;
	}

	if (!bAllowAnyPlayerForceStart)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Force start is not allowed by game settings", FString(__FUNCTION__));
		return;
	}

	FName RoomCode = NAME_None;
	if (!TryGetPlayerRoomCode(Requester, RoomCode))
	{
		UE_LOGFMT(LogProjectA, Log, "{0} — Requester is not in any room", FString(__FUNCTION__));
		return;
	}

	AssignRandomCharactersForRoom(RoomCode);
	StartMatchTravelForRoom(RoomCode);
}

void ALobbyGameMode::HandleLobbyStateChanged()
{
	for (auto& [RoomCode, Room] : RoomsByCode)
	{
		if (CanStartByReady(RoomCode))
		{
			StartCountdownForRoom(RoomCode);
		}
		else
		{
			CancelCountdownForRoom(RoomCode);
		}

		PushRoomStateToPlayers(RoomCode);
	}
		
	RefreshLobbyState();
}

void ALobbyGameMode::RefreshLobbyState()
{
	ALobbyGameState* LobbyGameState = GetGameState<ALobbyGameState>();
	if (!LobbyGameState)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — Failed to get lobby game state", FString(__FUNCTION__));
		return;
	}

	int32 ConnectedPlayers = 0;
	int32 ReadyPlayers = 0;
	bool bAnyMatchStarting = false;
	int32 MinCountdownSeconds = -1;
	TArray<FLobbyPlayerView> LobbyPlayersView{};
	TArray<FLobbyRoomView> LobbyRoomsView{};

	for (const auto& [RoomCode, Room] : RoomsByCode)
	{
		UpdateMatchStartingState(Room, bAnyMatchStarting, MinCountdownSeconds);

		int32 ValidPlayersInRoom = 0;
		AppendRoomPlayersToLobbyView(Room, ConnectedPlayers, ReadyPlayers, LobbyPlayersView, ValidPlayersInRoom);
		AppendLobbyRoomView(RoomCode, Room, ValidPlayersInRoom, LobbyRoomsView);
	}

	LobbyGameState->SetLobbySnapshot(
		ConnectedPlayers,
		ReadyPlayers,
		bAnyMatchStarting,
		MinCountdownSeconds,
		AvailableCharacterIds,
		LobbyPlayersView,
		LobbyRoomsView
	);
}

void ALobbyGameMode::PushRoomStateToPlayers(FName RoomCode)
{
	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — No room found for code {1}", FString(__FUNCTION__), *RoomCode.ToString());
		return;
	}

	TArray<FLobbyPlayerView> LobbyPlayers;
	GatherLobbyPlayersForRoom(*Room, LobbyPlayers);
	BroadcastRoomStateToPlayers(RoomCode, *Room, LobbyPlayers);
}

void ALobbyGameMode::StartCountdownForRoom(FName RoomCode)
{
	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} — No room found for code {1}", FString(__FUNCTION__), *RoomCode.ToString());
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(Room->CountdownTimerHandle))
	{
		return;
	}

	Room->bMatchStarting = true;
	Room->CurrentCountdownSeconds = CountdownSeconds;

	FTimerDelegate TimerDelegate{};
	TimerDelegate.BindUObject(this, &ALobbyGameMode::HandleRoomCountdownTick, RoomCode);
	GetWorldTimerManager().SetTimer(Room->CountdownTimerHandle, TimerDelegate, 1.0f, true, 0.0f);
}

void ALobbyGameMode::CancelCountdownForRoom(FName RoomCode)
{
	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room)
	{
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(Room->CountdownTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(Room->CountdownTimerHandle);
	}

	Room->bMatchStarting = false;
	Room->CurrentCountdownSeconds = -1;
}

void ALobbyGameMode::HandleRoomCountdownTick(FName RoomCode)
{
	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room)
	{
		return;
	}
		
	--Room->CurrentCountdownSeconds;

	if (Room->CurrentCountdownSeconds <= 0)
	{
		GetWorldTimerManager().ClearTimer(Room->CountdownTimerHandle);
		StartMatchTravelForRoom(RoomCode);
		return;
	}

	PushRoomStateToPlayers(RoomCode);
	RefreshLobbyState();
}

FName ALobbyGameMode::GenerateLobbyCode() const
{
	for (int32 Attempt = 0; Attempt < 100; ++Attempt)
	{
		const int32 Value = FMath::RandRange(1000, 9999);
		const FName Code(*FString::FromInt(Value));
		if (!RoomsByCode.Contains(Code))
		{
			return Code;
		}
	}
	return NAME_None;
}

bool ALobbyGameMode::CanStartByReady(FName RoomCode) const
{
	const FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room)
	{
		return false;
	}

	int32 ConnectedPlayers = 0;
	int32 ReadyPlayers = 0;

	for (const TWeakObjectPtr<ALobbyPlayerController>& PlayerControllerPtr : Room->Players)
	{
		const ALobbyPlayerController* PlayerController = PlayerControllerPtr.Get();
		if (!PlayerController)
		{
			continue;
		}

		const ALobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<ALobbyPlayerState>();
		if (!LobbyPlayerState)
		{
			continue;
		}

		++ConnectedPlayers;

		const bool bIsReady = LobbyPlayerState->IsReadyStatus();
		if (bIsReady)
		{
			++ReadyPlayers;
		}

		const FName SelectedCharacterId = LobbyPlayerState->GetSelectedCharacterId();
		if (SelectedCharacterId.IsNone() || !IsCharacterAllowed(SelectedCharacterId))
		{
			return false;
		}
	}

	return ConnectedPlayers >= MinPlayersToStart && ConnectedPlayers == ReadyPlayers;
}

bool ALobbyGameMode::TryGetPlayerRoomCode(ALobbyPlayerController* PlayerController, FName& OutRoomCode) const
{
	if (!PlayerController)
	{
		return false;
	}

	const FName* Found = RoomCodeByPlayer.Find(PlayerController);
	if (!Found || Found->IsNone())
	{
		return false;
	}

	OutRoomCode = *Found;
	return true;
}

bool ALobbyGameMode::IsCharacterAllowed(FName CharacterId) const
{
	return !CharacterId.IsNone() && AvailableCharacterIds.Contains(CharacterId);
}

void ALobbyGameMode::AssignRandomCharactersForRoom(FName RoomCode)
{
	FLobbyRoomRuntime* Room = RoomsByCode.Find(RoomCode);
	if (!Room || AvailableCharacterIds.Num() == 0)
	{
		return;
	}

	for (const TWeakObjectPtr<ALobbyPlayerController>& PcPtr : Room->Players)
	{
		ALobbyPlayerController* PlayerController = PcPtr.Get();
		if (!PlayerController)
		{
			continue;
		}

		ALobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<ALobbyPlayerState>();
		if (!LobbyPlayerState)
		{
			continue;
		}

		if (!LobbyPlayerState->GetSelectedCharacterId().IsNone())
		{
			continue;
		}

		const int32 RandomIndex = FMath::RandRange(0, AvailableCharacterIds.Num() - 1);
		const FName RandomCharacterId = AvailableCharacterIds[RandomIndex];
		LobbyPlayerState->SetSelectedCharacterId(RandomCharacterId);
	}
}

bool ALobbyGameMode::SetRoomConfig(ALobbyPlayerController* Requester, FName RoomCode)
{
	if (!Requester || RoomCode.IsNone())
	{
		return false;
	}

	FLobbyRoomRuntime& Room = RoomsByCode.FindOrAdd(RoomCode);
	Room.LobbyCode = RoomCode;
	Room.Players.AddUnique(Requester);
	Room.bMatchStarting = false;
	Room.CurrentCountdownSeconds = -1;

	RoomCodeByPlayer.Add(Requester, RoomCode);

	return true;
}




/// 
/// Helpers
/// 
bool ALobbyGameMode::TryFetchPlayerRoom(ALobbyPlayerController * Requester, FName & OutOldRoomCode, FLobbyRoomRuntime * &OutRoom) const
{
	if (!Requester)
	{
		return false;
	}

	if (!TryGetPlayerRoomCode(Requester, OutOldRoomCode))
	{
		OutRoom = nullptr;
		return false;
	}
	OutRoom = const_cast<FLobbyRoomRuntime*>(RoomsByCode.Find(OutOldRoomCode));
	return true;
}

void ALobbyGameMode::SendClientClearedRoomState(ALobbyPlayerController * Requester) const
{
	if (!Requester)
	{
		return;
	}

	TArray<FLobbyPlayerView> EmptyLobbyPlayers{};
	Requester->Client_UpdateLobbyRoomState(
		NAME_None,
		EmptyLobbyPlayers,
		false,
		-1,
		false,
		AvailableCharacterIds,
		NAME_None
		);
}

void ALobbyGameMode::RemovePlayerReferencesFromRoom(FLobbyRoomRuntime * Room, ALobbyPlayerController * Requester)
{
	if (!Room || !Requester)
	{
		return;
	}
	// Remove invalid entries and the requester
	Room->Players.RemoveAll([Requester](const TWeakObjectPtr<ALobbyPlayerController>& Item)
	{
		return !Item.IsValid() || Item.Get() == Requester;
	});
	
	// Remove reverse mapping
	RoomCodeByPlayer.Remove(Requester);
}

void ALobbyGameMode::ResetPlayerLobbyState(ALobbyPlayerController * Requester) const
{
	if (!Requester)
	{
		return;
	}
	
	if (ALobbyPlayerState* LobbyPlayerState = Requester->GetPlayerState<ALobbyPlayerState>())
	{
		LobbyPlayerState->SetLobbyCode(NAME_None);
		LobbyPlayerState->SetReadyStatus(false);
		LobbyPlayerState->SetSelectedCharacterId(NAME_None);
	}
}

void ALobbyGameMode::MaybeDestroyRoomIfEmpty(FName RoomCode, FLobbyRoomRuntime * Room)
{
	if (!Room)
	{
		return;
	}

	if (Room->Players.Num() == 0)
	{
		CancelCountdownForRoom(RoomCode);
		RoomsByCode.Remove(RoomCode);
	}
}

void ALobbyGameMode::UpdateMatchStartingState(const FLobbyRoomRuntime& Room, bool& bAnyMatchStarting, int32& MinCountdownSeconds) const
{
	if (!Room.bMatchStarting)
	{
		return;
	}

	bAnyMatchStarting = true;

	if (Room.CurrentCountdownSeconds < 0)
	{
		return;
	}

	if (MinCountdownSeconds < 0)
	{
		MinCountdownSeconds = Room.CurrentCountdownSeconds;
	}
	else
	{
		MinCountdownSeconds = FMath::Min(MinCountdownSeconds, Room.CurrentCountdownSeconds);
	}
}

void ALobbyGameMode::AppendRoomPlayersToLobbyView(const FLobbyRoomRuntime& Room, int32& ConnectedPlayers,
	int32& ReadyPlayers, TArray<FLobbyPlayerView>& LobbyPlayersView, int32& ValidPlayersInRoom) const
{
	int32 NextNumber = 1;

	for (const TWeakObjectPtr<ALobbyPlayerController>& PcPtr : Room.Players)
	{
		const ALobbyPlayerController* PlayerController = PcPtr.Get();
		if (!PlayerController)
		{
			continue;
		}

		const ALobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<ALobbyPlayerState>();
		if (!LobbyPlayerState)
		{
			continue;
		}

		++ConnectedPlayers;
		++ValidPlayersInRoom;

		if (LobbyPlayerState->IsReadyStatus())
		{
			++ReadyPlayers;
		}

		FLobbyPlayerView View{};
		View.PlayerNumber = NextNumber++;
		View.bIsReady = LobbyPlayerState->IsReadyStatus();
		View.PlayerId = LobbyPlayerState->GetPlayerId();
		LobbyPlayersView.Add(View);
	}
}

void ALobbyGameMode::AppendLobbyRoomView(FName RoomCode, const FLobbyRoomRuntime& Room,	int32 ValidPlayersInRoom,
	TArray<FLobbyRoomView>& LobbyRoomsView) const
{
	FLobbyRoomView RoomView{};
	RoomView.LobbyCode = RoomCode;
	RoomView.PlayersCount = ValidPlayersInRoom;
	RoomView.bMatchStarting = Room.bMatchStarting;
	LobbyRoomsView.Add(RoomView);
}


void ALobbyGameMode::GatherLobbyPlayersForRoom(const FLobbyRoomRuntime& Room, TArray<FLobbyPlayerView>& OutLobbyPlayers) const
{
	OutLobbyPlayers.Empty();
	int32 NextNumber = 1;

	for (const TWeakObjectPtr<ALobbyPlayerController>& PcPtr : Room.Players)
	{
		const ALobbyPlayerController* Pc = PcPtr.Get();
		if (!Pc)
		{
			continue;
		}

		const ALobbyPlayerState* Ps = Pc->GetPlayerState<ALobbyPlayerState>();
		if (!Ps)
		{
			continue;
		}

		FLobbyPlayerView View{};
		View.PlayerNumber = NextNumber++;
		View.PlayerId = Ps->GetPlayerId();
		View.bIsReady = Ps->IsReadyStatus();
		OutLobbyPlayers.Add(View);
	}
}

void ALobbyGameMode::BroadcastRoomStateToPlayers(FName RoomCode, const FLobbyRoomRuntime& Room, const TArray<FLobbyPlayerView>& LobbyPlayers) const
{
	for (const TWeakObjectPtr<ALobbyPlayerController>& PcPtr : Room.Players)
	{
		ALobbyPlayerController* Pc = PcPtr.Get();
		if (!Pc)
		{
			continue;
		}

		const ALobbyPlayerState* Ps = Pc->GetPlayerState<ALobbyPlayerState>();
		const FName SelectedCharacterId = Ps ? Ps->GetSelectedCharacterId() : NAME_None;
		const bool bLocalReady = Ps ? Ps->IsReadyStatus() : false;

		Pc->Client_UpdateLobbyRoomState(
			RoomCode,
			LobbyPlayers,
			Room.bMatchStarting,
			Room.CurrentCountdownSeconds,
			bLocalReady,
			AvailableCharacterIds,
			SelectedCharacterId
		);
	}
}
		