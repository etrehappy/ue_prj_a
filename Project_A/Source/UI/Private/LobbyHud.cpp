#include "LobbyHud.h"

#include "Blueprint/UserWidget.h"
#include "LobbyMainWidget.h"
#include "LobbyRoomWidget.h"

#include "ProjectALog.h"

void ALobbyHud::BeginPlay()
{
	Super::BeginPlay();

	CreateLobbyWidget();
	CreateLobbyRoomWidget();
	UpdateScreenVisibility(false);
}

bool ALobbyHud::CreateLobbyWidget()
{
	if (LobbyWidget)
	{
		return true;
	}

	if (!LobbyWidgetClass)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyWidgetClass is not set", FString(__FUNCTION__));
		return false;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return false;
	}

	LobbyWidget = CreateWidget<ULobbyWidget>(PlayerController, LobbyWidgetClass);
	if (!LobbyWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create LobbyWidget", FString(__FUNCTION__));
		return false;
	}

	LobbyWidget->OnCreateLobbyRequested.AddDynamic(this, &ALobbyHud::OnCreateLobbyRequested);
	LobbyWidget->OnJoinLobbyRequested.AddDynamic(this, &ALobbyHud::OnJoinLobbyRequested);
	LobbyWidget->AddToViewport();

	return true;
}

bool ALobbyHud::CreateLobbyRoomWidget()
{
	if (LobbyRoomWidget)
	{
		return true;
	}

	if (!LobbyRoomWidgetClass)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - LobbyRoomWidgetClass is not set", FString(__FUNCTION__));
		return false;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is nullptr", FString(__FUNCTION__));
		return false;
	}

	LobbyRoomWidget = CreateWidget<ULobbyRoomWidget>(PlayerController, LobbyRoomWidgetClass);
	if (!LobbyRoomWidget)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to create LobbyRoomWidget", FString(__FUNCTION__));
		return false;
	}

	LobbyRoomWidget->OnReadyRequested.AddDynamic(this, &ALobbyHud::OnReadyRequested);
	LobbyRoomWidget->OnForceStartRequested.AddDynamic(this, &ALobbyHud::OnForceStartRequested);
	LobbyRoomWidget->OnCharacterSelected.AddDynamic(this, &ALobbyHud::OnCharacterSelected);
	LobbyRoomWidget->OnLeaveLobbyRequested.AddDynamic(this, &ALobbyHud::OnLeaveLobbyRequested);
	LobbyRoomWidget->AddToViewport();

	return true;
}

void ALobbyHud::UpdateScreenVisibility(bool bInLobbyRoom)
{
	if (LobbyWidget)
	{
		LobbyWidget->SetVisibility(bInLobbyRoom ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	if (LobbyRoomWidget)
	{
		LobbyRoomWidget->SetVisibility(bInLobbyRoom ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		LobbyRoomWidget->OnRoomWidgetVisibilityChanged();
	}
}

void ALobbyHud::UpdateRoomState(FName LobbyCode, int32 ConnectedPlayers, int32 ReadyPlayers,
	bool bMatchStarting, int32 CountdownSeconds, bool bInReady,	const TArray<FName>& AvailableCharacterIds,
	FName SelectedCharacterId,	const TArray<FLobbyPlayerView>& LobbyPlayers)
{
	if (!LobbyWidget && !CreateLobbyWidget())
	{
		return;
	}

	if (!LobbyRoomWidget && !CreateLobbyRoomWidget())
	{
		return;
	}

	const bool bInLobbyRoom = !LobbyCode.IsNone();
	UpdateScreenVisibility(bInLobbyRoom);

	LobbyRoomWidget->UpdateRoomState(LobbyCode, LobbyPlayers, bMatchStarting,
		CountdownSeconds, bInReady, AvailableCharacterIds, SelectedCharacterId);
}

void ALobbyHud::UpdateMainWidget(const TArray<FLobbyRoomView>& LobbyRooms)
{
	if (!LobbyWidget && !CreateLobbyWidget())
	{
		return;
	}

	TArray<FName> LobbyCodes{};
	LobbyCodes.Reserve(LobbyRooms.Num());

	for (const FLobbyRoomView& RoomView : LobbyRooms)
	{
		if (!RoomView.LobbyCode.IsNone())
		{
			LobbyCodes.Add(RoomView.LobbyCode);
		}
	}

	LobbyWidget->UpdateWidget(LobbyCodes);
}