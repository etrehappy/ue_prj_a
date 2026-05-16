/*****************************************************************//**
 * \file   LobbyRoomWidget.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "LobbyPlayerView.h"
#include "LobbyPlayerListItemObject.h"

#include "LobbyRoomWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UListView;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyRoomReadyRequested, bool, bNewReadyStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyRoomForceStartRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyRoomCharacterSelected, FName, CharacterId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyRoomLeaveRequested);

/**
 * @class ULobbyRoomWidget
 * @brief Represents the room UI widget, responsible for displaying lobby information and handling user interactions within the lobby.
 */
UCLASS()
class UI_API ULobbyRoomWidget : public UBaseWidget
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	ULobbyRoomWidget() = default;
	virtual ~ULobbyRoomWidget() = default;

	void UpdateRoomState(FName LobbyCode, const TArray<FLobbyPlayerView>& LobbyPlayers,
		bool bMatchStarting, int32 CountdownSeconds, bool bLocalReady, 
		const TArray<FName>& AvailableCharacterIds,	FName SelectedCharacterId);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void SelectCharacterByIndex(int32 Index, UButton* ClickedButton);

	void RefreshCharacterSlots();
	void RefreshPlayersListView();
	void RefreshTexts();

	bool IsSecondPressForSelectedCharacter(const FName& CharacterId);


						/* === C++ member variables === */
private:
	FName CachedLobbyCode{NAME_None};
	TArray<FLobbyPlayerView> CachedLobbyPlayers{};
	TArray<FName> CachedAvailableCharacterIds{};
	FName CachedSelectedCharacterId{NAME_None};
	bool bLocalReadyState{false};
	bool bCachedMatchStarting{false};
	int32 CachedCountdownSeconds{-1};


						/* === Unreal Engine UFUNCTION === */

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Lobby", meta = (DisplayName = "OnVisibilityChanged"))
	void OnRoomWidgetVisibilityChanged();

private:
	UFUNCTION()
	void OnReadyClicked();

	UFUNCTION()
	void OnForceStartClicked();

	UFUNCTION()
	void OnLeaveLobbyClicked();

	UFUNCTION()
	void OnCharacterSlotAClicked();

	UFUNCTION()
	void OnCharacterSlotBClicked();


						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyRoomReadyRequested OnReadyRequested{};

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyRoomForceStartRequested OnForceStartRequested{};

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyRoomCharacterSelected OnCharacterSelected{};

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyRoomLeaveRequested OnLeaveLobbyRequested{};

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UListView> LobbyPlayersListView{};
	
// Text
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentLobbyCodeText{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayersStatusText{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyButtonText{};


// UButton
	/**
	 * @brief A simple way to choose a cahracter.
	 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> CharacterSlotButtonA{};

	/**
	 * @brief A simple way to choose a cahracter.
	 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> CharacterSlotButtonB{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ForceStartButton{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> LeaveLobbyButton{};

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UButton>	ActiveButton{};

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ULobbyPlayerListItemObject>> LobbyPlayerListItems{};
};
