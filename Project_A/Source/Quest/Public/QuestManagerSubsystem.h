/*****************************************************************//**
 * \file   QuestManagerSubsystem.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuestTypes.h"

#include "QuestManagerSubsystem.generated.h"

class APawn;
class UQuestDefinition;
class UQuestLogComponent;




/**
 * @brief Server-authoritative manager for quest operations.
 * Also stores quest definitions on both server and clients for UI/read-only access.
 */
UCLASS(Config = Game)
class QUEST_API UQuestManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;	

private:
	void LoadQuestDefinitionsFromConfig();
	void RegisterQuestDefinition(UQuestDefinition* QuestDefinition);

	/**
	 * @brief Filters the given recipients to only those that should receive quest updates
	 */
	void FilterRecipientsForQuestTurnIn(FName QuestId, TArray<APawn*>& OutRecipients) const;
	void FilterRecipientsForQuestAccept(FName QuestId, TArray<APawn*>& OutRecipients) const;
	void FilterRecipientsForQuestProgress(FName QuestId, TArray<APawn*>& OutRecipients) const;
		
	UQuestLogComponent* GetQuestLogComponent(const APawn* Pawn) const;

	/**
	 * @brief When SharingMode == Party, finds all party members of SourcePawn that should receive quest updates.
	 * 
	 * @param[in,out] SourcePawn The pawn initiating the quest update.
	 * @param[out] OutRecipients The array of party members.
	 * @return True even if only SourcePawn is a valid recipient, false if SourcePawn is invalid and no recipients were added.
	 */
	bool FindRecipients(APawn* SourcePawn, EQuestSharingMode SharingMode, TArray<APawn*>& OutRecipients) const;

	
						/* === Unreal Engine UFUNCTION === */
public:	
	UFUNCTION(BlueprintPure, Category = "Quest|Manager")
	const UQuestDefinition* GetQuestDefinition(FName QuestId) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest|Manager")
	bool TryAcceptQuest(APawn* RequesterPawn, FName QuestId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest|Manager")
	bool TryAdvanceObjective(APawn* InstigatorPawn, FName QuestId, FGameplayTag ObjectiveTag, int32 Delta);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest|Manager")
	bool TryTurnInQuest(APawn* RequesterPawn, FName QuestId);

	
						/* === Unreal Engine UPROPERTY === */
private:
	/**
	 * @brief Loaded quest definitions cache, available on both server and clients
	 */
	UPROPERTY()
	TMap<FName, TObjectPtr<UQuestDefinition>> QuestDefinitionsById{};

	/**
	 * @brief Quest definitions to autoload on subsystem initialization (server + client).
	 * Configure in DefaultGame.ini.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Quest|Manager")
	TArray<TSoftObjectPtr<UQuestDefinition>> StartupQuestDefinitions{};
};