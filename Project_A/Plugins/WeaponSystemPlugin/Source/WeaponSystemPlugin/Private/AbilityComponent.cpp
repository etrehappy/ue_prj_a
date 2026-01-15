#include "AbilityComponent.h"
#include "Net/UnrealNetwork.h"

#include "WeaponPluginLog.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

    CreateAbilities();

	// Logging abilities info

#if WITH_EDITOR

	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - BeginPlay: AbilityClasses={1}, Abilities={2}", FString(__FUNCTION__), AbilityClasses.Num(), Abilities.Num());

	for (UBaseAbility* A : Abilities)
	{
		if (A)
		{
			UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Ability: Class={1}, Tag={2}", FString(__FUNCTION__), *A->GetClass()->GetName(), *A->GetAbilityTag().ToString());
		}
	}

#endif // WITH_EDITOR

}

void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UAbilityComponent::HandleInputAction(const FName InputName, const ETriggerEvent Event) const
{
	Server_HandleInputAction(InputName, Event);
}

void UAbilityComponent::CreateAbilities()
{
	for (auto& AbilityClass : AbilityClasses)
	{
		if (!AbilityClass.Get()) { continue; }

		UBaseAbility* NewAbility = NewObject<UBaseAbility>(this, AbilityClass);

		if (!NewAbility)
		{
			UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Failed to create ability of class {1}", FString(__FUNCTION__), *AbilityClass->GetName());
			return;
		}

		NewAbility->InitAbility(this);
		Abilities.Add(NewAbility);
	}
}

void UAbilityComponent::Server_HandleInputAction_Implementation(const FName InputName, const ETriggerEvent TriggerEvent) const
{
	if(InputName.IsNone())
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - InputName is None, aborting.", FString(__FUNCTION__));
		return;
	}

	if(TriggerEvent == ETriggerEvent::None)
	{
		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - TriggerEvent is None, aborting.", FString(__FUNCTION__));
		return;
	}



#if WITH_EDITOR
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Server_HandleInputAction received InputName={1}, TriggerEvent={2}, AbilitiesCount={3}", FString(__FUNCTION__), *InputName.ToString(), StaticEnum<ETriggerEvent>()->GetValueAsString(TriggerEvent), Abilities.Num());
#endif // WITH_EDITOR



	for (UBaseAbility* Ability : Abilities)
	{
		if (Ability->GetInputActionName() == InputName)
		{
			Ability->HandleInput(TriggerEvent);
			return;
		}
	}

    UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Ability with input action {1} not found", FString(__FUNCTION__), *InputName.ToString());
}




//void UAbilityComponent::ActivateAbilityByTag(FGameplayTag AbilityTag)
//{
//	for (UBaseAbility* Ability : Abilities)
//	{
//		if (Ability->AbilityTag == AbilityTag)
//		{
//			Ability->Activate();
//			return;
//		}
//	}
//
//    UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Ability with tag {1} not found", FString(__FUNCTION__), *AbilityTag.ToString());
//}