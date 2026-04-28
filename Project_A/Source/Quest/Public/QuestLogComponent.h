/*****************************************************************//**
 * \file   QuestLogComponent.h
 * \brief  Default quest log component for tracking quest states and broadcasting changes to the UI.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestLogComponent.generated.h"

/**
 * @enum EQuestState
 * @brief Represents the different states a quest can be in. A simple implementation.
 */
UENUM(BlueprintType)
enum class EQuestState : uint8
{
	None UMETA(DisplayName = "None"),
	Accepted_InsertStone UMETA(DisplayName = "Accepted_InsertStone"),
	Completed_InsertStone UMETA(DisplayName = "Completed_InsertStone")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, EQuestState, NewState, FText, Description);

/**
 * @class UQuestLogComponent 
 * @brief Can be added to the player character to track quest. A base and simple implementation.
 *  
 * @todo In this example, we only have one quest. It can be extended to support multiple quests by using a TMap<EQuestID, EQuestState> or similar data structure to track the state of each quest separately.
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

protected:
	virtual void BeginPlay() override;
	void BroadcastQuestState() const;

						/* === Unreal Engine UFUNCTION === */
public:

// A temporary solution
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
	void AcceptInsertStoneQuest();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quest")
	void CompleteInsertStoneQuest();

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsInsertStoneQuestAccepted() const { return CurrentQuestState == EQuestState::Accepted_InsertStone; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsInsertStoneQuestCompleted() const { return CurrentQuestState == EQuestState::Completed_InsertStone; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	EQuestState GetQuestState() const { return CurrentQuestState; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	FText GetCurrentQuestText() const;

private:
	UFUNCTION()
	void OnRep_CurrentQuestState();
		
						/* === Unreal Engine UPROPERTY === */
public:
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestStateChanged OnQuestStateChanged{};	

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentQuestState)
	EQuestState CurrentQuestState{EQuestState::None};
};