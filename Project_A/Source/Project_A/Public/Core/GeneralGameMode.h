/*****************************************************************//**
 * \file   GeneralGameMode.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectNetworkSettings.h"

#include "GeneralGameMode.generated.h"



/**
 * @class AGeneralGameMode
 * @brief It is a base class for modes that are used in playable maps (not for menu).
 */
UCLASS()
class PROJECT_A_API AGeneralGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGeneralGameMode() = default;
	virtual ~AGeneralGameMode() = default;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual EServerWorldType GetMapIdentifier() const;

	UFUNCTION(BlueprintCallable, Category = "GeneralGameMode")
	virtual void ExitToDesktop();
	
};
