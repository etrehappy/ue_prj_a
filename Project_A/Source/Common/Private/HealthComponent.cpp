#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "WspDamageType.h"
#include "StatusEffect/StatusEffectsTags.h"

#include "ProjectALog.h"


UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialise();

	if(OwnerActor)
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
	}
	
}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, MaxHealth);
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::Initialise()
{
	OwnerActor = GetOwner();
	if(!OwnerActor)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - OwnerActor is null", FString(__FUNCTION__));
	}

	CurrentHealth = MaxHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	UE_LOGFMT(LogProjectA, Log, "{0} - Initialized. MaxHealth: {1} / CurrentHealth: {2}", FString(__FUNCTION__), MaxHealth, CurrentHealth);
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

void UHealthComponent::SetMaxHealth(float NewHealth)
{
	if (NewHealth <= 0)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - NewHealth is less than or equal to 0, ignoring", FString(__FUNCTION__));
		return;
	}

	MaxHealth = NewHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}


void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Not running on dedicated server, ignoring damage", FString(__FUNCTION__));
		return;
	}

	const auto* WspDamageType = Cast<UWspDamageType>(DamageType);
	if(!WspDamageType)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DamageType is not UWspDamageType", FString(__FUNCTION__));
		return;
	}

	const auto DamageTypeEnum = WspDamageType->GetDamageType();

	if(DamageTypeEnum == EInjuryType::Heal)
	{
		IncreaseHealth(Damage);
	}
	else if(DamageTypeEnum != EInjuryType::None)
	{
		const float DefenseMultiplier = 100.f / (100.f + Defense);
		const float EffectiveDamage = FMath::Max(Damage * IncomingDamageMultiplier * DefenseMultiplier, 0.f);

		DecreaseHealth(EffectiveDamage);
	}
	else
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - DamageType is None, ignoring damage", FString(__FUNCTION__));
	}
}

void UHealthComponent::DecreaseHealth(float DamageAmount)
{
	float NewHealth = CurrentHealth - DamageAmount;
	if (NewHealth <= 0.f)
	{
		NewHealth = 0.f;
	}

	if (CurrentHealth == NewHealth)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Health is already at 0 or damage had no effect", FString(__FUNCTION__));
		return;
	}
	
	CurrentHealth = NewHealth;
	OnDecreaseHealth.Broadcast(DamageAmount);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		ToKill();
	}

	UE_LOGFMT(LogProjectA, Log, "{0} - Damage: {1} / CurrentHealth: {2}", FString(__FUNCTION__), DamageAmount, CurrentHealth);	
}

void UHealthComponent::IncreaseHealth(float HealAmount)
{
	const float NewHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);

	if (CurrentHealth == NewHealth)
	{
		UE_LOGFMT(LogProjectA, Log, "{0} - Health is already at max or heal had no effect", FString(__FUNCTION__));
		return;
	}

	CurrentHealth = NewHealth;
	OnIncreaseHealth.Broadcast(HealAmount);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	UE_LOGFMT(LogProjectA, Log, "{0} - Healed: {1} / CurrentHealth: {2}", FString(__FUNCTION__), HealAmount, CurrentHealth);
}

void UHealthComponent::ToKill()
{
	OnDeath.Broadcast();
}

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::OnRep_MaxHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

bool UHealthComponent::CanHandleStatTag(const FGameplayTag& StatTag) const
{
	return SupportedStatTags.HasTag(StatTag);
}

void UHealthComponent::ApplyStatusEffectAction(const FEffectAction& Action)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Should only be called on dedicated server", FString(__FUNCTION__));
		return;
	}

	/* simple implementation */

	const float EffectiveValue = Action.Value;

	if (Action.TargetStat == StatusEffectsTags::DefenseTag)
	{
		switch (Action.Type)
		{
		case EModifierType::Add:
			Defense = FMath::Max(Defense + Action.Value, 0.f);
			break;

		case EModifierType::Multiply:
			Defense = FMath::Max(Defense * Action.Value, 0.f);
			break;

		default:
			UE_LOGFMT(LogProjectA, Warning, "{0} - Unsupported modifier type for Defense", FString(__FUNCTION__));
			break;
		}

		UE_LOGFMT(LogProjectA, Log, "{0} - Defense updated to {1}", FString(__FUNCTION__), Defense);
		return;
	}

	if (Action.TargetStat == StatusEffectsTags::HealthTag)
	{

		switch (Action.Type)
		{
		case EModifierType::Add:
			if (EffectiveValue >= 0.f)
			{
				IncreaseHealth(EffectiveValue);
			}
			else
			{
				DecreaseHealth(FMath::Abs(EffectiveValue));
			}
			break;

		case EModifierType::Multiply:
			SetCurrentHealth(CurrentHealth * EffectiveValue);
			break;

		default:
			UE_LOGFMT(LogProjectA, Warning, "{0} - Unsupported modifier type", FString(__FUNCTION__));
			break;
		}
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - Unsupported TargetStat {1}", FString(__FUNCTION__), Action.TargetStat.ToString());
	return;

}

void UHealthComponent::RemoveStatusEffectAction(const FEffectAction& Action)
{
	if (Action.TargetStat == StatusEffectsTags::DefenseTag)
	{	
		switch (Action.Type)
		{
		case EModifierType::Add:
			Defense = FMath::Max(Defense - Action.Value, 0.f);
			break;

		case EModifierType::Multiply:
			if (FMath::IsNearlyZero(Action.Value))
			{
				UE_LOGFMT(LogProjectA, Warning, "{0} - Invalid multiplier for revert", FString(__FUNCTION__));
				return;
			}

			Defense /= Action.Value;
			Defense = FMath::Max(Defense, 0.f);
			break;
		default:
			UE_LOGFMT(LogProjectA, Warning, "{0} - Unsupported modifier type", FString(__FUNCTION__));
			return;
		}

		UE_LOGFMT(LogProjectA, Log, "{0} - Defense reverted to {1}", FString(__FUNCTION__),
			Defense);
	}

	UE_LOGFMT(LogProjectA, Warning, "{0} - RemoveStatusEffectAction called but not supported for tag {1}",
		FString(__FUNCTION__), Action.TargetStat.ToString());

	return;
}

void UHealthComponent::SetCurrentHealth(float NewHealth)
{
	if (!IsRunningDedicatedServer())
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - Should only be called on dedicated server", FString(__FUNCTION__));
		return;
	}

	const float ClampedHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	if (CurrentHealth == ClampedHealth)
	{
		return;
	}

	CurrentHealth = ClampedHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		ToKill();
	}
}

