#include "NpcBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "StatusEffect/StatusEffectsComponent.h"
#include "NpcBattleComponent.h"
#include "DialogueDefinition.h"
#include "DialogueManagerSubsystem.h"
#include "InventoryComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "ProjectALog.h"

ANpcBase::ANpcBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetComponentTickEnabled(false);

	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusEffectComponent"));
	StatusEffectComponent->SetComponentTickEnabled(false);

	NpcBattleComponent = CreateDefaultSubobject<UNpcBattleComponent>(TEXT("NpcBattleComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetComponentTickEnabled(false);
}

void ANpcBase::DestroyNpc()
{
	Destroy();
}

void ANpcBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ANpcBase::OnDead);
		HealthComponent->OnIncreaseHealth.AddDynamic(this, &ANpcBase::OnIncreaseHealth);
		HealthComponent->OnDecreaseHealth.AddDynamic(this, &ANpcBase::OnDecreaseHealth);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void ANpcBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANpcBase, MoveSpeed);
	DOREPLIFETIME(ANpcBase, NpcFaction);
	DOREPLIFETIME(ANpcBase, bIsDead);
	DOREPLIFETIME(ANpcBase, CurrentTarget);
	DOREPLIFETIME(ANpcBase, NpcBattleComponent);
	DOREPLIFETIME(ANpcBase, bIsAttacking);
}

void ANpcBase::SetNpcCoreData(float InMaxHealth, float InDamage, float InMoveSpeed, ENpcFaction InFaction)
{
	if (!HasAuthority())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should be called only on authority", FString(__FUNCTION__));
		return;
	}

	if (HealthComponent && InMaxHealth > 0.f)
	{
		HealthComponent->SetMaxHealth(InMaxHealth);
	}

	if (NpcBattleComponent)
	{
		NpcBattleComponent->SetDamageAmount(InDamage);
	}

	MoveSpeed = InMoveSpeed;
	NpcFaction = InFaction;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void ANpcBase::SetCurrentTarget(AActor* NewTarget)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - should be called only on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	CurrentTarget = NewTarget;
	OnCurrentTargetChanged(CurrentTarget);
}

void ANpcBase::SetIsAttacking(bool bNewIsAttacking)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - SetIsAttacking should only be called on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return;
	}

	bIsAttacking = bNewIsAttacking;
}

void ANpcBase::OnDead()
{
	if (!IsRunningDedicatedServer())
	{
		return;
	}

	bIsDead = true;
	Multicast_PlayDeathFX();

	if (NpcBattleComponent)
	{
		NpcBattleComponent->StopAttack(FString(__FUNCTION__));
		NpcBattleComponent->SetComponentTickEnabled(false);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->ClearFocus(EAIFocusPriority::Gameplay);
		AICon->StopMovement();

		if (UBrainComponent* Brain = AICon->BrainComponent)
		{
			Brain->StopLogic(TEXT("Dead"));
			Brain->SetComponentTickEnabled(false);
		}

		if (UStateTreeAIComponent* StateTreeComponent = AICon->FindComponentByClass<UStateTreeAIComponent>())
		{
			StateTreeComponent->StopLogic(TEXT("Dead"));
			StateTreeComponent->SetComponentTickEnabled(false);
		}

		if (UAIPerceptionComponent* PerceptionComponent = AICon->FindComponentByClass<UAIPerceptionComponent>())
		{
			PerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(AICon);
			PerceptionComponent->SetComponentTickEnabled(false);
		}
	}

	if (UStateTreeComponent* PawnStateTreeComponent = FindComponentByClass<UStateTreeComponent>())
	{
		PawnStateTreeComponent->StopLogic(TEXT("Dead"));
		PawnStateTreeComponent->SetComponentTickEnabled(false);
	}

	bUseControllerRotationYaw = false;
	if (AController* NpcController = GetController())
	{
		NpcController->SetControlRotation(GetActorRotation());
	}

	if (DeathLifeSpan <= 0.f)
	{
		DestroyNpc();
	}
	else
	{
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ANpcBase::DestroyNpc, DeathLifeSpan, false);
	}
}

void ANpcBase::OnIncreaseHealth(float HealAmount)
{
	Multicast_PlayHealFX(HealAmount);
}

void ANpcBase::OnDecreaseHealth(float DamageAmount)
{
	Multicast_PlayDamageFX(DamageAmount);
}

void ANpcBase::OnRep_CurrentTarget()
{
	OnCurrentTargetChanged(CurrentTarget);
}

void ANpcBase::Multicast_PlayHealFX_Implementation(float HealAmount)
{
	if (IsRunningDedicatedServer())
	{
		return;
	}

	OnPlayHealFX(HealAmount);
}

void ANpcBase::Multicast_PlayDamageFX_Implementation(float DamageAmount)
{
	if (IsRunningDedicatedServer())
	{
		return;
	}

	OnPlayDamageFX(DamageAmount);
}

void ANpcBase::Multicast_PlayDeathFX_Implementation()
{
	if (IsRunningDedicatedServer())
	{
		return;
	}

	OnPlayDeathFX();
}

bool ANpcBase::CanInteract(APawn* Interactor) const
{
	if (!Interactor)
	{
		return false;
	}

	const bool bAliveRule = !bIsDead && bCanInteractWhenAlive;
	const bool bDeadRule = bIsDead && bCanInteractWhenDead;

	if (!bAliveRule && !bDeadRule)
	{
		return false;
	}

	return BP_CanInteract(Interactor);
}

bool ANpcBase::HasDirectDialogue() const
{
	return DialogueDefinition && !DialogueDefinition->DialogueId.IsNone();
}

bool ANpcBase::TryStartDialogueDirect(APawn* Interactor) const
{
	if (!Interactor || !HasAuthority() || !HasDirectDialogue())
	{
		return false;
	}

	UWorld* World = GetWorld();
	UDialogueManagerSubsystem* DialogueManagerSubsystem = World ? World->GetSubsystem<UDialogueManagerSubsystem>() : nullptr;
	if (!DialogueManagerSubsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DialogueManagerSubsystem is missing", FString(__FUNCTION__));
		return false;
	}

	FDialogueNodeRuntime OutNode{};
	const bool bStarted = DialogueManagerSubsystem->TryStartDialogue(Interactor, const_cast<ANpcBase*>(this), DialogueDefinition->DialogueId, OutNode);
	if (!bStarted)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Failed to start dialogue '{1}' for NPC: {2}",
			FString(__FUNCTION__), DialogueDefinition->DialogueId.ToString(), *GetName());
	}

	return bStarted;
}

void ANpcBase::BuildInteractionActions(APawn* Interactor, TArray<FInteractionActionType>& OutActions) const
{
	OutActions.Reset();

	if (!CanInteract(Interactor))
	{
		return;
	}

	// Direct dialogue path: do not show interaction menu, start dialogue immediately.
	if (HasDirectDialogue())
	{
		//TryStartDialogueDirect(Interactor);
		return;
	}

	// Legacy interaction menu path for NPCs without dialogue definition.
	for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
	{
		if (!ActionDef || !ActionDef->ActionTag.IsValid())
		{
			continue;
		}

		FInteractionActionType ActionView{};
		ActionView.bIsEnabled = ActionDef->bEnabledByDefault;
		ActionView.Definition = ActionDef;
		OutActions.Add(ActionView);
	}
}

bool ANpcBase::ExecuteInteractionAction(APawn* Interactor, FGameplayTag ActionTag)
{
	if (!CanInteract(Interactor))
	{
		return false;
	}

	// Explicit action path (used by interaction menu and dialogue effects).
	if (ActionTag.IsValid())
	{
		bool bHasEnabledActionInList = false;

		for (const TObjectPtr<UInteractionActionDefinition>& ActionDef : InteractionActions)
		{
			if (!ActionDef || !ActionDef->ActionTag.IsValid())
			{
				continue;
			}

			if (ActionDef->ActionTag.MatchesTagExact(ActionTag))
			{
				bHasEnabledActionInList = ActionDef->bEnabledByDefault;
				break;
			}
		}

		// For dialogue-driven NPCs allow explicit action tags from dialogue effects
		// even if they are not present in legacy InteractionActions array.
		if (!bHasEnabledActionInList && !HasDirectDialogue())
		{
			return false;
		}

		return BP_ExecuteInteractionAction(Interactor, ActionTag);
	}

	// Implicit interact path (e.g. press E without choosing explicit action).
	if (HasDirectDialogue())
	{
		return TryStartDialogueDirect(Interactor);
	}

	return false;
}

UInventoryComponent* ANpcBase::GetInventoryComponent() const
{
	if (!bSupportsInventoryInteraction)
	{
		return nullptr;
	}

	return InventoryComponent.Get();
}

bool ANpcBase::CanAcceptSharedTarget(const AActor* NewTarget) const
{
	if (!NewTarget || NewTarget == this)
	{
		return false;
	}

	if (bIsDead)
	{
		return false;
	}
		
	if (bIsAttacking && CurrentTarget && CurrentTarget != NewTarget)
	{
		return false;
	}

	return true;
}

bool ANpcBase::TryApplySharedTarget(AActor* NewTarget, const AActor* SignalSource)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA,  Warning, "{0} - should be called only on the server. Actor: {1}", FString(__FUNCTION__), *GetName());
		return false;
	}

	if (!CanAcceptSharedTarget(NewTarget))
	{
		return false;
	}
		
	if (CurrentTarget == NewTarget)
	{
		return true;
	}

	SetCurrentTarget(NewTarget);
	return true;
}