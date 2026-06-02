#include "NpcAlertSubsystem.h"

#include "NpcBase.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Math/UnrealMathUtility.h"
#include "NpcBattleComponent.h"

#include "ProjectALog.h"

bool UNpcAlertSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	if (!World)
	{
		return false;
	}

	const bool bIsGameWorld = World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE;
	if (!bIsGameWorld)
	{
		return false;
	}
		
	return IsRunningDedicatedServer();
}

void UNpcAlertSubsystem::Deinitialize()
{
	LastBroadcastTimeByNpc.Reset();
	Super::Deinitialize();
}
//
//void UNpcAlertSubsystem::BroadcastTargetSpotted(ANpcBase* SignalerNpc, AActor* SpottedTarget)
//{
//	UWorld* World = GetWorld();
//	if (!World || !IsRunningDedicatedServer())
//	{
//		return;
//	}
//
//	if (!SignalerNpc || !SpottedTarget || SignalerNpc == SpottedTarget || SignalerNpc->IsDead())
//	{
//		return;
//	}
//
//	CleanupInvalidCooldownEntries();
//
//	const double CurrentTimeSeconds = World->GetTimeSeconds();
//
//	// Throttle per-target broadcasts
//	{
//		const TWeakObjectPtr<AActor> TargetKey{SpottedTarget};
//		const double* LastTargetTimePtr = LastAlertTimeByTarget.Find(TargetKey);
//		if (LastTargetTimePtr && (CurrentTimeSeconds - *LastTargetTimePtr) < TargetRepeatCooldown)
//		{
//			// recent broadcast for this target - skip
//			return;
//		}
//	}
//
//	if (!CanBroadcastNow(SignalerNpc, CurrentTimeSeconds))
//	{
//		return;
//	}
//
//	const float AlertRadiusSquared = FMath::Square(AlertRadius);
//	int32 AppliedCount = 0;
//
//	for (TActorIterator<ANpcBase> It(World); It; ++It)
//	{
//		ANpcBase* CandidateNpc = *It;
//		if (!CandidateNpc || CandidateNpc == SignalerNpc)
//		{
//			continue;
//		}
//
//		// skip dead or different faction
//		if (CandidateNpc->IsDead())
//		{
//			continue;
//		}
//
//		if (CandidateNpc->GetNpcFaction() != SignalerNpc->GetNpcFaction())
//		{
//			continue;
//		}
//
//		// distance check
//		const float DistanceSquared = FVector::DistSquared(SignalerNpc->GetActorLocation(), CandidateNpc->GetActorLocation());
//		if (DistanceSquared > AlertRadiusSquared)
//		{
//			continue;
//		}
//
//		// If candidate already has different target -> skip (we don't want to switch engaged attackers)
//		AActor* ExistingTarget = CandidateNpc->GetCurrentTarget();
//		if (ExistingTarget && ExistingTarget != SpottedTarget)
//		{
//			// additionally, if candidate is attacking someone else, skip (configurable)
//			if (bSkipIfCurrentlyAttacking)
//			{
//				// try find battle component to check attacking state
//				if (UNpcBattleComponent* BattleComp = CandidateNpc->FindComponentByClass<UNpcBattleComponent>())
//				{
//					if (BattleComp->IsAttacking())
//					{
//						continue;
//					}
//				}
//				else
//				{
//					// if no battle component, rely on current target check only - continue already done
//					continue;
//				}
//			}
//			else
//			{
//				continue;
//			}
//		}
//
//		// optional: require line of sight from candidate to target
//		if (bRequireLineOfSight)
//		{
//			FVector Start = CandidateNpc->GetActorLocation() + FVector(0.f, 0.f, 50.f);
//			FVector End = SpottedTarget->GetActorLocation() + FVector(0.f, 0.f, 50.f);
//
//			FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(NpcAlert_LOS), true);
//			TraceParams.AddIgnoredActor(CandidateNpc);
//			TraceParams.AddIgnoredActor(SignalerNpc);
//			TraceParams.AddIgnoredActor(SpottedTarget);
//
//			// If any blocking hit exists, then no LOS
//			if (World->LineTraceTestByChannel(Start, End, ECC_Visibility, TraceParams))
//			{
//				continue;
//			}
//		}
//
//		// ask candidate if it can accept the shared target
//		if (!CandidateNpc->CanAcceptSharedTarget(SpottedTarget))
//		{
//			continue;
//		}
//
//		// try to apply. TryApplySharedTarget returns false if failed or refused.
//		if (!CandidateNpc->TryApplySharedTarget(SpottedTarget, SignalerNpc))
//		{
//			continue;
//		}
//
//		++AppliedCount;
//		if (AppliedCount >= MaxReceiversPerBroadcast)
//		{
//			break;
//		}
//	}
//
//	// mark per-signaler and per-target times
//	MarkBroadcastTime(SignalerNpc, CurrentTimeSeconds);
//	LastAlertTimeByTarget.Add(TWeakObjectPtr<AActor>(SpottedTarget), CurrentTimeSeconds);
//
//	UE_LOGFMT(
//		LogProjectA,
//		Log,
//		"{0} - Signaler: {1}, Target: {2}, AppliedCount: {3}",
//		FString(__FUNCTION__),
//		*SignalerNpc->GetName(),
//		*SpottedTarget->GetName(),
//		AppliedCount);
//}

bool UNpcAlertSubsystem::CanBroadcastNow(ANpcBase* SignalerNpc, double CurrentTimeSeconds) const
{
	if (!SignalerNpc)
	{
		return false;
	}

	const TWeakObjectPtr<ANpcBase> Key{SignalerNpc};
	const double* LastTimePtr = LastBroadcastTimeByNpc.Find(Key);
	if (!LastTimePtr)
	{
		return true;
	}

	return (CurrentTimeSeconds - *LastTimePtr) >= MinBroadcastIntervalSeconds;
}

void UNpcAlertSubsystem::MarkBroadcastTime(ANpcBase* SignalerNpc, double CurrentTimeSeconds)
{
	if (!SignalerNpc)
	{
		return;
	}

	LastBroadcastTimeByNpc.Add(TWeakObjectPtr<ANpcBase>(SignalerNpc), CurrentTimeSeconds);
}

void UNpcAlertSubsystem::CleanupInvalidCooldownEntries()
{
	TArray<TWeakObjectPtr<ANpcBase>> KeysToRemove{};
	for (const TPair<TWeakObjectPtr<ANpcBase>, double>& Pair : LastBroadcastTimeByNpc)
	{
		if (!Pair.Key.IsValid())
		{
			KeysToRemove.Add(Pair.Key);
		}
	}

	for (const TWeakObjectPtr<ANpcBase>& Key : KeysToRemove)
	{
		LastBroadcastTimeByNpc.Remove(Key);
	}
}

void UNpcAlertSubsystem::BroadcastTargetSpotted(ANpcBase* SignalerNpc, AActor* SpottedTarget)
{
	// 1. Validation
	UWorld* World = GetWorld();
	if (!World || !IsRunningDedicatedServer())	{ return; }

	if (!SignalerNpc || !SpottedTarget || SignalerNpc == SpottedTarget 
		|| SignalerNpc->IsDead()) { return;	}

	CleanupInvalidCooldownEntries();
	const double CurrentTimeSeconds = World->GetTimeSeconds();

	if (HasRecentAlertForTarget(SpottedTarget, CurrentTimeSeconds))	{ return; }
		
	if (!CanBroadcastNow(SignalerNpc, CurrentTimeSeconds)) { return; }

	const float AlertRadiusSquared = FMath::Square(AlertRadius);
	int32 AppliedCount = 0;

	// iterate and notify eligible candidates
	NotifyCandidates(SignalerNpc, SpottedTarget, AlertRadiusSquared, /*out*/ AppliedCount);

	// mark per-signaler and per-target times
	MarkBroadcastTime(SignalerNpc, CurrentTimeSeconds);
	LastAlertTimeByTarget.Add(TWeakObjectPtr<AActor>(SpottedTarget), CurrentTimeSeconds);
}


bool UNpcAlertSubsystem::HasRecentAlertForTarget(AActor* SpottedTarget, double CurrentTimeSeconds) const
{
	if (!SpottedTarget)	{ return true; }

	const TWeakObjectPtr<AActor> TargetKey{SpottedTarget};
	const double* LastTargetTimePtr = LastAlertTimeByTarget.Find(TargetKey);
	if (LastTargetTimePtr && (CurrentTimeSeconds - *LastTargetTimePtr) < TargetRepeatCooldown)
	{
		// recent broadcast for this target - skip
		return true;
	}

	return false;
}

void UNpcAlertSubsystem::NotifyCandidates(ANpcBase* SignalerNpc, AActor* SpottedTarget, float AlertRadiusSquared, int32& OutAppliedCount) const
{
	OutAppliedCount = 0;
	UWorld* World = GetWorld();
	if (!World)	{ return; }

	for (TActorIterator<ANpcBase> It(World); It; ++It)
	{
		ANpcBase* CandidateNpc = *It;
		if (!CandidateNpc || CandidateNpc == SignalerNpc) { continue; }

		if (!IsCandidateEligible(CandidateNpc, SignalerNpc, AlertRadiusSquared)) { continue; }

		// line of sight check may require world & target
		if (bRequireLineOfSight && !HasLineOfSightToTarget(CandidateNpc, SpottedTarget, SignalerNpc)) { continue; }

		// ask candidate if it can accept the shared target and try to apply
		if (!TryNotifyCandidate(CandidateNpc, SignalerNpc, SpottedTarget)) { continue; }

		++OutAppliedCount;
		if (OutAppliedCount >= MaxReceiversPerBroadcast) { break; }
	}
}

bool UNpcAlertSubsystem::IsCandidateEligible(ANpcBase* CandidateNpc, ANpcBase* SignalerNpc, float AlertRadiusSquared) const
{
	if (!CandidateNpc || !SignalerNpc) { return false; }
		
	if (CandidateNpc->IsDead()) { return false;	}

	if (CandidateNpc->GetNpcFaction() != SignalerNpc->GetNpcFaction()) { return false; }

	// distance check
	const float DistanceSquared = FVector::DistSquared(SignalerNpc->GetActorLocation(), CandidateNpc->GetActorLocation());
	if (DistanceSquared > AlertRadiusSquared) { return false; }

	// If candidate already has different target -> skip
	AActor* ExistingTarget = CandidateNpc->GetCurrentTarget();
	bool bHasDifferentTarget = ExistingTarget && ExistingTarget != SignalerNpc->GetCurrentTarget();

	if (bHasDifferentTarget)
	{
		// additionally, if candidate is attacking someone else, skip (configurable)
		if (!bSkipIfCurrentlyAttacking) { return false; }

		UNpcBattleComponent* BattleComp = CandidateNpc->FindComponentByClass<UNpcBattleComponent>();

		if (!BattleComp) { return false; }

		if (BattleComp->IsAttacking()) { return false; }
		
	}

	return true;
}

bool UNpcAlertSubsystem::HasLineOfSightToTarget(ANpcBase* CandidateNpc, AActor* SpottedTarget, ANpcBase* SignalerNpc) const
{
	if (!CandidateNpc || !SpottedTarget || !SignalerNpc) { return false; }

	UWorld* World = GetWorld();
	if (!World)	{ return false; }

	FVector Start = CandidateNpc->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	FVector End = SpottedTarget->GetActorLocation() + FVector(0.f, 0.f, 50.f);

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(NpcAlert_LOS), true);
	TraceParams.AddIgnoredActor(CandidateNpc);
	TraceParams.AddIgnoredActor(SignalerNpc); // SignalerNpc is not available here; ignore candidate only
	TraceParams.AddIgnoredActor(SpottedTarget);

	// If any blocking hit exists, then no LOS
	return !World->LineTraceTestByChannel(Start, End, ECC_Visibility, TraceParams);
}

bool UNpcAlertSubsystem::TryNotifyCandidate(ANpcBase* CandidateNpc, ANpcBase* SignalerNpc, AActor* SpottedTarget) const
{
	if (!CandidateNpc)	{ return false;	}
		
	if (!CandidateNpc->CanAcceptSharedTarget(SpottedTarget)) { return false; }
		
	return CandidateNpc->TryApplySharedTarget(SpottedTarget, SignalerNpc);
}