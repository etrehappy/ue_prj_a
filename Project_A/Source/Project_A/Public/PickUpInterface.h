

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "PickUpInterface.generated.h"

class AItemPickup;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickUpInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_A_API IPickUpInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool PickUpItem(AItemPickup* Item);


};
