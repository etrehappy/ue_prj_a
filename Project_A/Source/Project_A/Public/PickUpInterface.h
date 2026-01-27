

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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void UpdatedCurrentPickUpItem(AItemPickup* Item) = 0;
	virtual void CleanCurrentPickUpItem() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PickUpItem(AItemPickup* Item);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void ShowPopup(AItemPickup* Item);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void HidePopup();
};
