/*****************************************************************//**
 * \file   Interactable.h
 * \brief  Simple implementation of an interface for interactable objects.
 * 
 * \date   February 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionActionTypes.h"

#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * @interface IInteractable
 * @brief Interface for interactable objects. Classes that implement this interface can be interacted with by a player character or other actors. 
 */
class INTERACTION_API IInteractable
{
	GENERATED_BODY()

public:
	
    virtual bool CanInteract(APawn* Interactor) const { return false; };

    virtual void Interact(APawn* Interactor) = 0;

    virtual void BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
    {
        OutActions.Reset();
    }

    virtual bool ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag) { return false; }

    /**
     * @brief Gets the maximum distance at which the interactor can interact with this object.
     * 
     * @return The interaction distance.
	 * @todo Consider making this a property that can be set in the editor.
     */
    virtual float GetInteractionDistance() const { return 200.f; }

    /**
     * @brief Gets the maximum angle at which the interactor can interact with this object.
     * 
     * @return The interaction angle.
	 * @todo Consider making this a property that can be set in the editor.
     */
    virtual float GetInteractionAngle() const { return 60.f; }
};
