
/*****************************************************************//**
 * \file   LobbyMainWidget.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "LobbyPlayerView.h"
#include "UObject/Object.h"

#include "LobbyMainWidget.generated.h"

class UButton;
class UTextBlock;
class UListView;
class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyCreateRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyJoinRequested, FName, LobbyCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyRoomBrowserJoinRequested, FName, LobbyCode);

/**
 * @class ULobbyRoomBrowserListItemObject
 * @brief Represents a single item in the room list.
 * This class is used to display the room code and handle join requests.
 */
UCLASS(BlueprintType)
class UI_API ULobbyRoomBrowserListItemObject : public UObject
{
	GENERATED_BODY()

public:
	void Initialise(FName InLobbyCode);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestJoin();

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyRoomBrowserJoinRequested OnJoinRequested{};

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	FName LobbyCode{NAME_None};
};

/**
 * @class ULobbyWidget
 * @brief 
 */
UCLASS()
class UI_API ULobbyWidget : public UBaseWidget
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	ULobbyWidget() = default;
	virtual ~ULobbyWidget() = default;

	/**
	 * @brief Updates the lobby widget with the current list of lobby codes. 
	 * It is called by the HUD when the lobby state changes.
	 * 
	 * @param[in] LobbyCodes
	 */
	void UpdateWidget(const TArray<FName>& LobbyCodes);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	/**
	 * @brief Helper function.
	 * @see UpdateWidget
	 */
	void RefreshRoomsListView();

	/**
	 * @brief Helper function.
	 * @see UpdateWidget
	 */
	void RefreshTexts();


						/* === C++ member variables === */
private:
	TArray<FName> CachedLobbyCodes{};
	bool bInLobbyRoom{false};


						/* === Unreal Engine UFUNCTION === */
protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void OnHidden();

private:
	UFUNCTION()
	void OnCreateRoomClicked();

	UFUNCTION()
	void OnJoinRoomRequested(FName LobbyCode);

	UFUNCTION()
	void OnJoinRoomClicked();


						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable)
	FOnLobbyCreateRequested OnCreateLobbyRequested{};

	UPROPERTY(BlueprintAssignable)
	FOnLobbyJoinRequested OnJoinLobbyRequested{};

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> CreateLobbyButton{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UListView> LobbyRoomsListView{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> LobbyCodeInput{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> JoinLobbyButton{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmptyRoomsText{};

	UPROPERTY(Transient)
	TArray<TObjectPtr<ULobbyRoomBrowserListItemObject>> LobbyRoomListItems{};
};