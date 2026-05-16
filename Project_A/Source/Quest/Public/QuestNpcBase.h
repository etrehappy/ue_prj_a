/*****************************************************************//**
 * \file   QuestNpcBase.h
 * \brief  It expends NPC base class to implement quest-related interactions.
 *
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "NpcBase.h"
#include "Interactable.h"

#include "QuestNpcBase.generated.h"

/**
 * @class AQuestNpcBase 
 * @brief Implements quest-related interactions for NPCs. 
 */
UCLASS()
class QUEST_API AQuestNpcBase : public ANpcBase, public IInteractable
{
	GENERATED_BODY()

public:
	virtual bool CanInteract(APawn* Interactor) const override;

	/**
	 * @brief A simple implementation of the Interact function that accepts a quest for the interactor.
	 */
	virtual void Interact(APawn* Interactor) override;

	virtual float GetInteractionDistance() const override { return InteractionDistance; }
	virtual float GetInteractionAngle() const override { return InteractionAngle; }

	virtual void BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const override;
	virtual bool ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	float InteractionDistance{250.f};

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	float InteractionAngle{70.f};

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TArray<TObjectPtr<UInteractionActionDefinition>> InteractionActions{};
};