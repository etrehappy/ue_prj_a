
/*****************************************************************//**
 * \file   QuestInteractableActor.h
 * \brief  A wrapper for interacable actors related to quests.
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"

#include "QuestInteractableActor.generated.h"

class UQuestObjectiveSourceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestInteractionExecuted, APawn*, Interactor);

/**
 * @brief It is a base class for quest-related interactable actors. It can be used for various types of interactable objects related to quests, such as quest items, quest-related environment objects, etc. It provides a simple implementation of the IInteractable interface and can be extended to add more specific functionality for different types of quest interactables.
 */
UCLASS()
class QUEST_API AQuestInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AQuestInteractableActor();
	virtual ~AQuestInteractableActor() override = default;

	virtual bool CanInteract(APawn* Interactor) const override; // IInteractable
	virtual void BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const override; // IInteractable
	virtual bool ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag) override; // IInteractable

	virtual float GetInteractionDistance() const override { return InteractionDistance; } // IInteractable
	virtual float GetInteractionAngle() const override { return InteractionAngle; } // IInteractable

public:
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestInteractionExecuted OnQuestInteractionExecuted{};

protected:
	/**
	 * @brief Component that provides quest objective functionality for this actor.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UQuestObjectiveSourceComponent> QuestObjectiveSourceComponent{};

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionDistance{250.f};

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionAngle{70.f};
};