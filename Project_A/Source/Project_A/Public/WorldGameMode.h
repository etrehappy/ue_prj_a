

#pragma once

#include "CoreMinimal.h"
#include "GeneralGameMode.h"

#include "WorldGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API AWorldGameMode : public AGeneralGameMode
{
	GENERATED_BODY()
	
public:
	AWorldGameMode();
	virtual ~AWorldGameMode() = default;

	virtual EServerWorldType GetMapIdentifier() const override;	
};
