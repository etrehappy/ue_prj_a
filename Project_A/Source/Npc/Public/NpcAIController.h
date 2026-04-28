/*****************************************************************//**
 * \file   NpcAIController.h
 * \brief  Default AI controller for NPCs.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NpcAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UStateTreeAIComponent;

/**
 * @see ANpcAIController
 * @brief In the current implementation, this class updates a current target of a NPC based on perceived actors and their hostility.
 */
UCLASS()
class NPC_API ANpcAIController : public AAIController
{
	GENERATED_BODY()
						/* === C++ member functions === */
public:
	ANpcAIController();
	virtual ~ANpcAIController() = default;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	bool IsHostileActor(const AActor* Actor) const;

	/**
	 * @brief Clears a current target of a NPC.
	 */
	void ClearCurrentTarget();

						/* === Unreal Engine UFUNCTION === */
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnCurrentTargetCleared();


private:
	/**
	 * @brief Checks if a perceived actor is hostile and updates a current target of a NPC.
	 */
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


						/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAISenseConfig_Sight> SightConfig{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent{};

};