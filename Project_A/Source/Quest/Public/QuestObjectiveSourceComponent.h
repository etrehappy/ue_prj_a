/*****************************************************************//**
 * \file   QuestObjectiveSourceComponent.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InteractionActionTypes.h"

#include "QuestObjectiveSourceComponent.generated.h"

class APawn;
class UInteractionActionDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveSourceConsumed);

/**
 * @brief Component that represents a source of quest objectives. It can be attached to actors to provide quest-related interactions. 
 */
UCLASS(ClassGroup = (Quest), meta = (BlueprintSpawnableComponent))
class QUEST_API UQuestObjectiveSourceComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UQuestObjectiveSourceComponent();
	virtual ~UQuestObjectiveSourceComponent() override = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Builds a list of interaction actions that this objective source provides. This can be used to display available interactions to the player. 
	 * 	 
	 * @param[out] OutActions The list of interaction actions that are available to the interactor.
	 */
	void BuildActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const;
	bool TryExecute(APawn* Interactor, FGameplayTag ActionTag);

private:
	bool CanExecute(APawn* Interactor) const;
	bool IsActionAllowed(FGameplayTag ActionTag) const;


						/* === Unreal Engine UFUNCTION === */
protected:
	UFUNCTION()
	void OnRep_IsConsumed();

	/**
	 * @brief It is called when the objective source is consumed (e.g. an item is collected, an enemy is killed). It can be used to trigger visual/audio feedback, spawn effects, etc. It is a BlueprintImplementableEvent, so it can be implemented in Blueprints to provide specific behavior for different types of objective sources.
	 *
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnObjectiveSourceConsumed();


						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnObjectiveSourceConsumed OnConsumed;

protected:
	/**
	 * @brief It is the ID of the quest this objective source is related to. It should be the same as the QuestId in UQuestDefinition.
	 * @see UQuestDefinition
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FName QuestId{NAME_None};

	/**
	 * @brief It is the tag of the objective this source is related to. It should be the same as the ObjectiveTag in UQuestDefinition.
	 * @see UQuestDefinition
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FGameplayTag ObjectiveTag{};

	/**
	 * @brief How much progress this objective source provides when executed. For example, if the objective is "Kill 5 goblins" and this source is a goblin, the tag can be "KillGoblin" and the delta can be 1. The quest system will update the quest instance by adding the delta to the current progress of the objective.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest", meta = (ClampMin = "1"))
	int32 ObjectiveDelta{1};

	/**
	 * @brief Whether the quest must be accepted to interact with this objective source. 
	 * If false, an interactable item starts quest after interaction.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	bool bRequireQuestAccepted{true};

	/**
	 * @brief Whether this objective source can only be executed once. 
	 * If true, it will be marked as consumed after execution and cannot be executed again. 
	 * If false, it can be executed multiple times to provide progress for repeatable objectives.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	bool bOneShot{false};

	/**
	 * @brief A list of interaction actions that this objective source provides. Used by HUD
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TArray<TObjectPtr<UInteractionActionDefinition>> InteractionActions{};

	/**
	 * @brief Whether this objective source consumes a required item when executed. 
	 * If true, the required item will be consumed from the interactor's inventory upon execution.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest|Cost")
	bool bConsumeRequiredItem{false};

	/**
	 * @brief An item's id that is required to interact with this objective source. 
	 * It should be the same as the tag of the item in the inventory system.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest|Cost")
	FGameplayTag RequiredItemTag{};

	/**
	 * @brief How many of the required item are needed to interact with this objective source. For example, if the objective is "Collect 3 herbs" and this source is a herb, the tag can be "Herb" and the count can be 3. The quest system will check if the interactor has enough of the required item before allowing interaction, and will consume the items if bConsumeRequiredItem is true.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Quest|Cost", meta = (ClampMin = "1"))
	int32 RequiredItemCount{1};

	UPROPERTY(ReplicatedUsing = OnRep_IsConsumed)
	bool bIsConsumed{false};
};