#include "StatusEffect/StatusEffectsComponent.h"

#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "StatusEffect/StatusEffectSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "StatusEffect/StatusEffectStatHandler.h"

#include "ProjectALog.h"

UStatusEffectComponent::UStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = IsRunningDedicatedServer();	
	PrimaryComponentTick.TickInterval = 0.2f; // 5 time per second
	SetIsReplicatedByDefault(true);
}

void UStatusEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckActiveEffects(DeltaTime);
}

void UStatusEffectComponent::CheckActiveEffects(float DeltaTime)
{
	if (!IsRunningDedicatedServer() || ActiveEffects.IsEmpty())	{ return; }

	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& Effect = ActiveEffects[i];

		Effect.RemainingTime -= DeltaTime;
		Effect.TimeAccumulator += DeltaTime;

		const bool bIsPeriodic = Effect.Spec.bIsPeriodic && Effect.Spec.Period > 0.f;
		if (bIsPeriodic)
		{
			while (Effect.TimeAccumulator >= Effect.Spec.Period)
			{
				Effect.TimeAccumulator -= Effect.Spec.Period;

				for (const FEffectAction& Action : Effect.Spec.Actions)
				{
					ApplyAction(Action);
				}
			}
		}

		const bool bShouldExpire = Effect.RemainingTime <= 0.f;

		if (bShouldExpire)
		{
			if (Effect.Spec.bRevertOnExpire && Effect.bAppliedOnStart)
			{
				RemoveInstantActions(Effect);
			}

			ActiveEffects.RemoveAt(i);
		}

	}

	if (ActiveEffects.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

void UStatusEffectComponent::ApplyEffectByTag(FGameplayTag EffectTag)
{
	if (!IsRunningDedicatedServer())
	{
		Server_ApplyEffectByTag(EffectTag);
		return;
	}

	UStatusEffectSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStatusEffectSubsystem>();

	if (!Subsystem)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Could not get UStatusEffectSubsystem from GameInstance", *FString(__FUNCTION__));
		return;
	}
	
	const auto* Definition = Subsystem->GetEffectByTag(EffectTag);
	
	if (!Definition)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Could not find effect definition for tag {1}", *FString(__FUNCTION__), EffectTag.ToString());
		return;
	}
	
	ApplyEffectSpec(Definition);
}

void UStatusEffectComponent::Server_ApplyEffectByTag_Implementation(FGameplayTag EffectTag)
{	
	if (!EffectTag.IsValid())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid EffectTag provided: {1}", *FString(__FUNCTION__), EffectTag.ToString());
		return;
	}

	ApplyEffectByTag(EffectTag);
}

void UStatusEffectComponent::ApplyEffectSpec(const FStatusEffectDef* Spec)
{
	if (!Spec)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid FStatusEffectDef provided", *FString(__FUNCTION__));
		return;
	}

	SetComponentTickEnabled(true);

	if(UpdateActiveEffect(Spec)) { return; }

	// else apply as new effect

	FActiveStatusEffect NewEffect;
	NewEffect.Spec = *Spec;
	NewEffect.RemainingTime = Spec->Duration;
	NewEffect.TimeAccumulator = 0.f;
	NewEffect.StackCount = 1;

	if (Spec->bIsPeriodic) 
	{ 
		ActiveEffects.Add(MoveTemp(NewEffect)); 
	}
	else if (Spec->bRevertOnExpire && Spec->Duration > 0.f) 
	{ 
		ApplyInstantActions(NewEffect); 
		NewEffect.bAppliedOnStart = true; 
		ActiveEffects.Add(MoveTemp(NewEffect)); 
	}
	else 
	{ 
		ApplyInstantActions(NewEffect); 
	}
}

void UStatusEffectComponent::ApplyInstantActions(FActiveStatusEffect& Effect)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - ApplyInstantActions should only be called on the server", *FString(__FUNCTION__));
		return;
	}

	for (const FEffectAction& Action : Effect.Spec.Actions)
	{
		ApplyAction(Action);	
	}
}

void UStatusEffectComponent::ApplyAction(const FEffectAction& Action)
{
	
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - ApplyInstantActions should only be called on the server", *FString(__FUNCTION__));
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", *FString(__FUNCTION__));
		return;
	}

	auto Componentse = Owner->GetComponents();
	TInlineComponentArray<UActorComponent*> Components(Owner);

	for (UActorComponent* Component : Components)
	{		
		IStatusEffectStatHandler* Handler{};
		if (!CanHandleStatTag(Component, Action, Handler)) { continue; }

		Handler->ApplyStatusEffectAction(Action);

		return;
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - No stat handler found for tag {1}", *FString(__FUNCTION__), Action.TargetStat.ToString());
}

bool UStatusEffectComponent::CanHandleStatTag(UActorComponent* Component, const FEffectAction& Action, IStatusEffectStatHandler*& OutHandler) const
{
	const bool bIsInvalidComponent = !Component || !Component->GetClass()->ImplementsInterface(UStatusEffectStatHandler::StaticClass());
	if (bIsInvalidComponent) { return false; }


	OutHandler = Cast<IStatusEffectStatHandler>(Component);
	if (!OutHandler) { return false; }


	if (!OutHandler->CanHandleStatTag(Action.TargetStat)) { return false; }

	return true;
}

void UStatusEffectComponent::RemoveInstantActions(const FActiveStatusEffect& Effect) 
{
	if (!IsRunningDedicatedServer()) 
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} - RemoveInstantActions should only be called on the server", *FString(__FUNCTION__));
		return; 
	}

	for (const FEffectAction& Action : Effect.Spec.Actions)
	{
		RemoveAction(Action);
	}
}

void UStatusEffectComponent::RemoveAction(const FEffectAction& Action) 
{
	if (!IsRunningDedicatedServer()) 
	{ 
		UE_LOGFMT(LogProjectA, Warning, "{0} - RemoveAction should only be called on the server", *FString(__FUNCTION__)); 
		return; 
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Owner is null", *FString(__FUNCTION__));
		return;
	}

	TInlineComponentArray<UActorComponent*> Components(Owner);

	for (UActorComponent* Component : Components)
	{
		IStatusEffectStatHandler* Handler{};
		if (!CanHandleStatTag(Component, Action, Handler)) { continue; }

		Handler->RemoveStatusEffectAction(Action);

		return;
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - No stat handler found for tag {1}", *FString(__FUNCTION__), Action.TargetStat.ToString());
}

bool UStatusEffectComponent::UpdateActiveEffect(const FStatusEffectDef* Spec)
{
	if (!Spec)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid FStatusEffectDef provided", *FString(__FUNCTION__));
		return false;
	}

	FActiveStatusEffect* ExistingEffect = nullptr;
	for (FActiveStatusEffect& Effect : ActiveEffects)
	{
		if (Effect.Spec.EffectTag == Spec->EffectTag)
		{
			ExistingEffect = &Effect;
			break;
		}
	}

	if (!ExistingEffect) { return false; }
		
	if (Spec->bRefreshDurationOnReapply)
	{
		ExistingEffect->RemainingTime = Spec->Duration;
	}

	// stack
	if (ExistingEffect->StackCount < Spec->MaxStacks)
	{
		ExistingEffect->StackCount++;
	}

	// Activ effect was updated. New effect doesn't need to be applied, so we return true.
	return true;	
}


	
