/*****************************************************************//**
 * \file   QuestLogComponent.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "QuestDefinition.h"
#include "QuestTypes.h"

#include "QuestLogComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestLogUpdated);

class UInventoryComponent;

/**
 * @class UQuestLogComponent
 * @brief Component that stores runtime quest instances for the owner actor.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUEST_API UQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UQuestLogComponent();
	virtual ~UQuestLogComponent() = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Returns a runtime struct that contains the current state of a quest for a specific player.
	 * @see FQuestInstance
	 */
	const FQuestInstance* GetQuestInstance(FName QuestId) const;

protected:
	virtual void BeginPlay() override;

private:
	void BroadcastQuestState() const;
	void ValidateTrackedQuest();

	const FQuestInstance* FindQuestInstance(FName QuestId) const;
	FQuestInstance* FindQuestInstanceMutable(FName QuestId);

	/**
	 * @see OwnerInventoryComponent
	 */
	void BindInventoryEvents();
	void SyncInventoryBackedObjectives();
	void StartInventoryBindRetry();

	const FQuestObjectiveDefinition* FindObjectiveDefinitionByTag(const UQuestDefinition* QuestDefinition, const FGameplayTag& ObjectiveTag);
	

						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Returns an array of runtime structs that contain the current state of all quests for a specific player.
	 * @see FQuestInstance	 
	 */
	UFUNCTION(BlueprintPure, Category = "Quest")
	const TArray<FQuestInstance>& GetQuestInstances() const { return QuestInstances; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
	void AcceptQuestFromDefinition(const FQuestInstance& InitialInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
	bool UpdateObjectiveProgress(FName QuestId, FGameplayTag ObjectiveTag, int32 Delta);

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsQuestAccepted(FName QuestId) const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsQuestCompleted(FName QuestId) const;
		
	/**
	 * @brief A client's preference. Determines which quest's objectives are shown in the UI tracker.
	 * @see UQuestJournalWidget
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Quest")
	void SetTrackedQuestId(FName InQuestId);

	/**
	 * @brief It is required for UI.
	 * @see UQuestJournalWidget
	 */
	UFUNCTION(BlueprintPure, Category = "Quest")
	FName GetTrackedQuestId() const { return TrackedQuestId; }

	/**
	 * @brief Returns an any quest for tracking via UI.
	 */
	UFUNCTION(BlueprintPure, Category = "Quest")
	FName GetTrackedOrFirstAcceptedQuestId() const;

	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
	bool TryTurnInQuest(FName QuestId);

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsQuestReadyToTurnIn(FName QuestId) const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsQuestTurnedIn(FName QuestId) const;

	/**
	 * @brief A simple solution to restore a character's state in a quest when he died.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
	void RestoreFromSnapshot(const TArray<FQuestInstance>& Snapshot);

private:
	UFUNCTION()
	void OnRep_QuestInstances();

	/**
	 * @see OwnerInventoryComponent
	 */
	UFUNCTION()
	void RetryBindInventoryEvents();

	/**
	 * @see OwnerInventoryComponent	 
	 */
	UFUNCTION()
	void HandleOwnedInventoryChanged();


						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestLogUpdated OnQuestLogUpdated{};

private:
	UPROPERTY(ReplicatedUsing = OnRep_QuestInstances)
	TArray<FQuestInstance> QuestInstances{};

	/**
	 * @brief Local UI preference (not replicated) 
	 * @see UQuestJournalWidget
	 */
	UPROPERTY()
	FName TrackedQuestId{NAME_None};
	
	/**
	 * @brief When quest objectives are backed by inventory items, we need to bind to inventory events to keep them in sync.
	 */
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwnerInventoryComponent{};
	
	/**
	 * @brief If the inventory component isn't ready when we try to bind to it, we'll retry after a short delay. 
	 */
	UPROPERTY()
	FTimerHandle InventoryBindRetryTimer{};
};