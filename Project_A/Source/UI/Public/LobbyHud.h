/*****************************************************************//**
 * \file   LobbyHud.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyPlayerView.h"

#include "LobbyHud.generated.h"

class ULobbyWidget;
class ULobbyRoomWidget;

/**
 * @class ALobbyHud
 * @brief The HUD class for the lobby server. 
 * It manages the main lobby widget and the room widget, 
 * and updates them based on the current state of the lobby and the room.
 */
UCLASS()
class UI_API ALobbyHud : public AHUD
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	ALobbyHud() = default;
	virtual ~ALobbyHud() = default;

	/**
	 * @brief Updates the main lobby widget with the current list of lobby rooms.
	 * 
	 * @param[in] LobbyRooms The list of lobby rooms to display.
	 * @see LobbyWidget
	 */
	void UpdateMainWidget(const TArray<FLobbyRoomView>& LobbyRooms);

	/**
	 * @brief Updates the room widget with the current state of the room.
	 * 
	 * @see LobbyRoomWidget
	 */
	void UpdateRoomState(FName LobbyCode, int32 ConnectedPlayers, int32 ReadyPlayers,
		bool bMatchStarting, int32 CountdownSeconds, bool bInReady, const TArray<FName>& AvailableCharacterIds,
		FName SelectedCharacterId, const TArray<FLobbyPlayerView>& LobbyPlayers);

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * @brief Helper function.
	 */
	bool CreateLobbyWidget();

	/**
	 * @brief Helper function.
	 */
	bool CreateLobbyRoomWidget();

	/**
	 * @brief Updates the visibility of the lobby and room widgets based on the current state.
	 * If the player is in a lobby room, the lobby widget should be hidden and the room widget should be shown. 
	 * If the player is not in a lobby room, the lobby widget should be shown and the room widget should be hidden.
	 * 
	 * @param[in] bInLobbyRoom Whether the player is currently in a lobby room.
	 */
	void UpdateScreenVisibility(bool bInLobbyRoom);


						/* === Unreal Engine UFUNCTION === */
protected:
	// These functions call PlayerController's functions to send requests to the server.

	UFUNCTION(BlueprintImplementableEvent)
	void OnReadyRequested(bool bNewReadyStatus);

	UFUNCTION(BlueprintImplementableEvent)
	void OnForceStartRequested();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCharacterSelected(FName CharacterId);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCreateLobbyRequested();

	UFUNCTION(BlueprintImplementableEvent)
	void OnJoinLobbyRequested(FName LobbyCode);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLeaveLobbyRequested();


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief The class of the main lobby.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass{};

	/**
	 * @brief The class of the room in the lobby-server.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<ULobbyRoomWidget> LobbyRoomWidgetClass{};

	/**
	 * @brief The instance of the main lobby widget.
	 */
	UPROPERTY(Transient)
	TObjectPtr<ULobbyWidget> LobbyWidget{};

	/**
	 * @brief The instance of the room widget in the lobby-server.
	 */
	UPROPERTY(Transient)
	TObjectPtr<ULobbyRoomWidget> LobbyRoomWidget{};

};