/*****************************************************************//**
 * \file   NpcAlertSubsystem.h
 * \brief  
 * 
 * \date   May 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "NpcAlertSubsystem.generated.h"


class ANpcBase;
class AActor;

/**
 * @brief Server's NPC alert.
 * Receives "target spotted" events and propagates target to nearby allied NPCs.
 */
UCLASS(Config = Game)
class NPC_API UNpcAlertSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Deinitialize() override;

private:
	bool CanBroadcastNow(ANpcBase* SignalerNpc, double CurrentTimeSeconds) const;
	void MarkBroadcastTime(ANpcBase* SignalerNpc, double CurrentTimeSeconds);
	void CleanupInvalidCooldownEntries();

	// helpers extracted to simplify BroadcastTargetSpotted
	bool HasRecentAlertForTarget(AActor* SpottedTarget, double CurrentTimeSeconds) const;
	void NotifyCandidates(ANpcBase* SignalerNpc, AActor* SpottedTarget, float AlertRadiusSquared, int32& OutAppliedCount) const;
	bool IsCandidateEligible(ANpcBase* CandidateNpc, ANpcBase* SignalerNpc, float AlertRadiusSquared) const;
	bool HasLineOfSightToTarget(ANpcBase* CandidateNpc, AActor* SpottedTarget, ANpcBase* SignalerNpc) const;
	bool TryNotifyCandidate(ANpcBase* CandidateNpc, ANpcBase* SignalerNpc, AActor* SpottedTarget) const;


						/* === C++ member variables === */
private:
	/**
	 * @brief Protects against spamming broadcasts from the same signaler NPC.
	 */
	TMap<TWeakObjectPtr<ANpcBase>, double> LastBroadcastTimeByNpc{};

private:
	/** 
	 * @brief Protects against spamming broadcasts for the same target. When many different NPCs spot the same target at the same time.
	 */
	TMap<TWeakObjectPtr<AActor>, double> LastAlertTimeByTarget{};


						/* === Unreal Engine UFUNCTION === */
public:
	/**
	 * @brief Called on server when a watcher NPC spots a hostile target.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Npc|Alert")
	void BroadcastTargetSpotted(ANpcBase* SignalerNpc, AActor* SpottedTarget);

	
						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief Max distance for alert propagation.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Npc|Alert", meta = (ClampMin = "100.0"))
	float AlertRadius{2000.f};

	/**
	 * @brief Safety limit per one broadcast tick.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Npc|Alert", meta = (ClampMin = "1", ClampMax = "128"))
	int32 MaxReceiversPerBroadcast{12};

	/**
	 * @brief Anti-spam cooldown.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Npc|Alert", meta = (ClampMin = "0.0"))
	float MinBroadcastIntervalSeconds{0.75f};

	/**
	 * @brief Require an unobstructed line of sight from candidate NPC to the spotted target.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Npc|Alert")
	bool bRequireLineOfSight{false};

	/**
	 * @brief If true, skip candidate NPCs that are currently attacking another target.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Npc|Alert")
	bool bSkipIfCurrentlyAttacking{true};

	/**
	 * @brief Minimum time between broadcasts for the same target.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Npc|Alert", meta = (ClampMin = "0.0"))
	float TargetRepeatCooldown{1.0f};
};