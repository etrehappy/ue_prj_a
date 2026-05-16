#include "LobbyRoomWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "LobbyPlayerListItemObject.h"

#include "ProjectALog.h"

void ULobbyRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReadyButton) { ReadyButton->OnClicked.AddDynamic(this, &ULobbyRoomWidget::OnReadyClicked); }
	if (ForceStartButton) { ForceStartButton->OnClicked.AddDynamic(this, &ULobbyRoomWidget::OnForceStartClicked); }
	if (LeaveLobbyButton) { LeaveLobbyButton->OnClicked.AddDynamic(this, &ULobbyRoomWidget::OnLeaveLobbyClicked); }

	if (CharacterSlotButtonA) { CharacterSlotButtonA->OnClicked.AddDynamic(this, &ULobbyRoomWidget::OnCharacterSlotAClicked); }
	if (CharacterSlotButtonB) { CharacterSlotButtonB->OnClicked.AddDynamic(this, &ULobbyRoomWidget::OnCharacterSlotBClicked); }

	RefreshCharacterSlots();
	RefreshPlayersListView();
	RefreshTexts();
}

void ULobbyRoomWidget::NativeDestruct()
{
	if (ReadyButton) { ReadyButton->OnClicked.RemoveDynamic(this, &ULobbyRoomWidget::OnReadyClicked); }
	if (ForceStartButton) { ForceStartButton->OnClicked.RemoveDynamic(this, &ULobbyRoomWidget::OnForceStartClicked); }
	if (LeaveLobbyButton) { LeaveLobbyButton->OnClicked.RemoveDynamic(this, &ULobbyRoomWidget::OnLeaveLobbyClicked); }

	if (CharacterSlotButtonA) { CharacterSlotButtonA->OnClicked.RemoveDynamic(this, &ULobbyRoomWidget::OnCharacterSlotAClicked); }
	if (CharacterSlotButtonB) { CharacterSlotButtonB->OnClicked.RemoveDynamic(this, &ULobbyRoomWidget::OnCharacterSlotBClicked); }

	Super::NativeDestruct();
}

void ULobbyRoomWidget::UpdateRoomState(
	FName LobbyCode,
	const TArray<FLobbyPlayerView>& LobbyPlayers,
	bool bMatchStarting,
	int32 CountdownSeconds,
	bool bLocalReady,
	const TArray<FName>& AvailableCharacterIds,
	FName SelectedCharacterId)
{	
	CachedLobbyCode = LobbyCode;
	CachedLobbyPlayers = LobbyPlayers;
	CachedAvailableCharacterIds = AvailableCharacterIds;
	CachedSelectedCharacterId = SelectedCharacterId;
	bCachedMatchStarting = bMatchStarting;
	CachedCountdownSeconds = CountdownSeconds;
	bLocalReadyState = bLocalReady;

	RefreshCharacterSlots();
	RefreshPlayersListView();
	RefreshTexts();
}

void ULobbyRoomWidget::OnReadyClicked()
{
	const bool bNewReadyStatus = !bLocalReadyState;
	OnReadyRequested.Broadcast(bNewReadyStatus);
}

void ULobbyRoomWidget::OnForceStartClicked()
{
	OnForceStartRequested.Broadcast();
}

void ULobbyRoomWidget::OnLeaveLobbyClicked()
{
	OnLeaveLobbyRequested.Broadcast();
}

void ULobbyRoomWidget::OnCharacterSlotAClicked()
{
	SelectCharacterByIndex(0, CharacterSlotButtonA);
}

void ULobbyRoomWidget::OnCharacterSlotBClicked()
{
	SelectCharacterByIndex(1, CharacterSlotButtonB);
}

void ULobbyRoomWidget::SelectCharacterByIndex(int32 Index, UButton* ClickedButton)
{
	if (!CachedAvailableCharacterIds.IsValidIndex(Index)) { return;	}

	const FName CharacterId = CachedAvailableCharacterIds[Index];
	if (CharacterId.IsNone()) { return;	}

	if (IsSecondPressForSelectedCharacter(CharacterId))	{ return; }
	
	CachedSelectedCharacterId = CharacterId;
	OnCharacterSelected.Broadcast(CharacterId);
}

void ULobbyRoomWidget::RefreshCharacterSlots()
{
	const bool bInLobbyRoom = !CachedLobbyCode.IsNone();
	const bool bHasA = CachedAvailableCharacterIds.IsValidIndex(0) && !CachedAvailableCharacterIds[0].IsNone();
	const bool bHasB = CachedAvailableCharacterIds.IsValidIndex(1) && !CachedAvailableCharacterIds[1].IsNone();

	if (CharacterSlotButtonA)
	{
		CharacterSlotButtonA->SetIsEnabled(bInLobbyRoom && bHasA);
	}
	if (CharacterSlotButtonB)
	{
		CharacterSlotButtonB->SetIsEnabled(bInLobbyRoom && bHasB);
	}
}

void ULobbyRoomWidget::RefreshPlayersListView()
{
	if (!LobbyPlayersListView)
	{
		return;
	}

	LobbyPlayersListView->ClearListItems();
	LobbyPlayerListItems.Reset();

	for (const FLobbyPlayerView& PlayerView : CachedLobbyPlayers)
	{
		ULobbyPlayerListItemObject* ItemObject = NewObject<ULobbyPlayerListItemObject>(this);
		if (!ItemObject)
		{
			continue;
		}

		ItemObject->Initialise(PlayerView);
		LobbyPlayerListItems.Add(ItemObject);
		LobbyPlayersListView->AddItem(ItemObject);
	}
}

void ULobbyRoomWidget::RefreshTexts()
{
	if (CurrentLobbyCodeText)
	{
		CurrentLobbyCodeText->SetText(
			CachedLobbyCode.IsNone()
			? FText::FromString(TEXT("Lobby: -"))
			: FText::FromString(FString::Printf(TEXT("Lobby: %s"), *CachedLobbyCode.ToString()))
		);
	}

	int32 ReadyPlayers = 0;
	for (const FLobbyPlayerView& PlayerView : CachedLobbyPlayers)
	{
		if (PlayerView.bIsReady)
		{
			++ReadyPlayers;
		}
	}

	if (PlayersStatusText)
	{
		PlayersStatusText->SetText(
			FText::FromString(FString::Printf(TEXT("Players: %d / Ready: %d"), CachedLobbyPlayers.Num(), ReadyPlayers))
		);
	}

	if (CountdownText)
	{
		if (bCachedMatchStarting && CachedCountdownSeconds >= 0)
		{
			CountdownText->SetText(FText::FromString(FString::Printf(TEXT("Match starts in: %d"), CachedCountdownSeconds)));
		}
		else
		{
			CountdownText->SetText(FText::FromString(TEXT("Waiting for players...")));
		}
	}

	const bool bInLobbyRoom = !CachedLobbyCode.IsNone();
	// ReadyButton доступна только если игрок в лобби и выбран персонаж
	const bool bCanReady = bInLobbyRoom && !CachedSelectedCharacterId.IsNone();

	if (ReadyButton)
	{
		ReadyButton->SetIsEnabled(bCanReady);
	}
	if (ForceStartButton)
	{
		ForceStartButton->SetIsEnabled(bInLobbyRoom);
	}
	if (LeaveLobbyButton)
	{
		LeaveLobbyButton->SetIsEnabled(bInLobbyRoom);
	}

	if (ReadyButtonText)
	{
		ReadyButtonText->SetText(bLocalReadyState ? FText::FromString(TEXT("Unready")) : FText::FromString(TEXT("Ready")));	
	}
}

bool ULobbyRoomWidget::IsSecondPressForSelectedCharacter(const FName& CharacterId)
{	
	if (CharacterId == CachedSelectedCharacterId)
	{		
		CachedSelectedCharacterId = NAME_None;

		if (bLocalReadyState)
		{
			OnReadyClicked();
		}
		OnCharacterSelected.Broadcast(CachedSelectedCharacterId);
		return true;
	}

	return false;
}

