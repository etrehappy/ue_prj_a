/*****************************************************************//**
 * \file   ClientStartGameMode.h
 * \brief  Default GameMode for the first window on the client, which will be used to connect to the hub server.
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GeneralGameMode.h"
#include "ClientStartGameMode.generated.h"

class ULoginWidget;

/**
 * @class AClientStartGameMode
 * @brief
 */
UCLASS(Config = NetSetCustom)
class PROJECT_A_API AClientStartGameMode : public AGeneralGameMode
{
	GENERATED_BODY()
	
public:
	AClientStartGameMode() = default;
	virtual ~AClientStartGameMode() = default;
	
	virtual EServerWorldType GetMapIdentifier() const override;

	UFUNCTION(BlueprintCallable)
	void ConnectToHubServer();

	UFUNCTION(BlueprintCallable)
	void ConnectToLobbyServer();	

protected:
	UPROPERTY(EditDefaultsOnly, Config, Category = "Network")
	FString HubServerAddress{};
};
