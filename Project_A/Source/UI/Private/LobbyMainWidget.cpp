
#include "LobbyMainWidget.h"

#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"

///////////////////////////////////////////////////////////////////////////////

void ULobbyRoomBrowserListItemObject::Initialise(FName InLobbyCode)
{
	LobbyCode = InLobbyCode;
}

void ULobbyRoomBrowserListItemObject::RequestJoin()
{
	if (!LobbyCode.IsNone())
	{
		OnJoinRequested.Broadcast(LobbyCode);
	}
}



///////////////////////////////////////////////////////////////////////////////

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CreateLobbyButton)
	{
		CreateLobbyButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnCreateRoomClicked);
	}

	if (JoinLobbyButton)
	{
		JoinLobbyButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnJoinRoomClicked);
	}

	RefreshRoomsListView();
	RefreshTexts();
}

void ULobbyWidget::NativeDestruct()
{
	if (CreateLobbyButton)
	{
		CreateLobbyButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::OnCreateRoomClicked);
	}

	if (JoinLobbyButton)
	{
		JoinLobbyButton->OnClicked.RemoveDynamic(this, &ULobbyWidget::OnJoinRoomClicked);
	}

	for (ULobbyRoomBrowserListItemObject* ItemObject : LobbyRoomListItems)
	{
		if (!ItemObject)
		{
			continue;
		}

		ItemObject->OnJoinRequested.RemoveDynamic(this, &ULobbyWidget::OnJoinRoomRequested);
	}

	LobbyRoomListItems.Reset();

	if (LobbyRoomsListView)
	{
		LobbyRoomsListView->ClearListItems();
	}

	Super::NativeDestruct();
}

void ULobbyWidget::UpdateWidget(const TArray<FName>& LobbyCodes)
{
	CachedLobbyCodes = LobbyCodes;
	RefreshRoomsListView();
	RefreshTexts();
}


void ULobbyWidget::OnCreateRoomClicked()
{
	OnCreateLobbyRequested.Broadcast();
}

void ULobbyWidget::OnJoinRoomRequested(FName LobbyCode)
{
	if (LobbyCode.IsNone())
	{
		return;
	}

	OnJoinLobbyRequested.Broadcast(LobbyCode);
}

void ULobbyWidget::OnJoinRoomClicked()
{
	if (!LobbyCodeInput)
	{
		return;
	}

	const FString Entered = LobbyCodeInput->GetText().ToString().TrimStartAndEnd();
	if (Entered.IsEmpty())
	{
		return;
	}

	// Broadcast as FName to reuse existing handlers
	OnJoinLobbyRequested.Broadcast(FName(*Entered));
}

void ULobbyWidget::RefreshRoomsListView()
{
	if (!LobbyRoomsListView)
	{
		return;
	}

	LobbyRoomsListView->ClearListItems();

	for (ULobbyRoomBrowserListItemObject* RoomRow : LobbyRoomListItems)
	{
		if (!RoomRow)
		{
			continue;
		}

		RoomRow->OnJoinRequested.RemoveDynamic(this, &ULobbyWidget::OnJoinRoomRequested);
	}

	LobbyRoomListItems.Reset();

	for (const FName LobbyCode : CachedLobbyCodes)
	{
		if (LobbyCode.IsNone())
		{
			continue;
		}

		ULobbyRoomBrowserListItemObject* RoomRow = NewObject<ULobbyRoomBrowserListItemObject>(this);
		if (!RoomRow)
		{
			continue;
		}

		RoomRow->Initialise(LobbyCode);
		RoomRow->OnJoinRequested.AddDynamic(this, &ULobbyWidget::OnJoinRoomRequested);

		LobbyRoomListItems.Add(RoomRow);
		LobbyRoomsListView->AddItem(RoomRow);
	}
}

void ULobbyWidget::RefreshTexts()
{
	if (CreateLobbyButton)
	{
		CreateLobbyButton->SetIsEnabled(!bInLobbyRoom);
	}

	if (JoinLobbyButton)
	{
		const bool bHasInput = LobbyCodeInput && !LobbyCodeInput->GetText().IsEmpty();
		JoinLobbyButton->SetIsEnabled(!bInLobbyRoom/* && bHasInput*/);
	}

	if (EmptyRoomsText)
	{
		const bool bShowEmpty = !bInLobbyRoom && (CachedLobbyCodes.Num() == 0);
		EmptyRoomsText->SetVisibility(bShowEmpty ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}