

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GeneralGameMode.generated.h"

UENUM(BlueprintType)
enum class EServerWorldType : uint8
{
	None			UMETA(DisplayName = "None")
	, Test			UMETA(DisplayName = "Test")
	, Hub			UMETA(DisplayName = "Hub")
	, MainWorld		UMETA(DisplayName = "MainWorld")
};

/**
 * 
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
	
};
