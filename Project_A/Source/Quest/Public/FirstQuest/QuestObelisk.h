/*****************************************************************//**
 * \file   QuestObelisk.h
 * \brief  
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interactable.h"
#include "QuestObelisk.generated.h"

/**
 * @brief A temporary solution for one quest. It should be changed.
 */
UCLASS()
class QUEST_API AQuestObelisk : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AQuestObelisk();
	virtual ~AQuestObelisk() override = default;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract(APawn* Interactor) const override;
	virtual void Interact(APawn* Interactor) override;

protected:
	UFUNCTION()
	void OnRep_IsActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnObeliskActivated();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated{false};

private:
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FGameplayTag RequiredStoneItemTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Quest", meta = (ClampMin = "1"))
	int32 RequiredStoneCount{1};

	
};