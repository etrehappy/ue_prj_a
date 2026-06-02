
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractionInventoryProvider.generated.h"

class UInventoryComponent;

UINTERFACE(MinimalAPI)
class UInteractionInventoryProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * @interface IInteractionInventoryProvider
 * @brief Contract for interactable objects that expose an inventory for interaction.
 */
class INTERACTION_API IInteractionInventoryProvider
{
	GENERATED_BODY()

public:
	/**
	 * @brief Returns inventory component that should be used for interaction (loot/trade/container).
	 * @return Valid inventory component or nullptr if unavailable.
	 */
	virtual UInventoryComponent* GetInventoryComponent() const = 0;
};