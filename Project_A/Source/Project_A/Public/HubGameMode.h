

#pragma once

#include "CoreMinimal.h"
#include "GeneralGameMode.h"

#include "HubGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API AHubGameMode : public AGeneralGameMode
{
	GENERATED_BODY()
	
public:
	AHubGameMode();
	virtual ~AHubGameMode() = default;
	
	void EnterToWorld(APlayerController* PC);
	virtual EServerWorldType GetMapIdentifier() const override;	
};
