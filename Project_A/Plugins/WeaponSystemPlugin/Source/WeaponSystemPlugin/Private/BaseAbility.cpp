


#include "BaseAbility.h"
#include "AbilityComponent.h"
#include "WeaponComponent.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "WeaponThrowable.h"

#include "WeaponPluginLog.h"

void UBaseAbility::InitAbility(UAbilityComponent* Owner)
{
    if (!Owner)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Owner is nullptr for ability '{1}'. Initialization aborted.", FString(__FUNCTION__), AbilityTag.ToString());
		return;
    }

    OwnerAbilityComponent = Owner;

    if (InputActionObjForName)
    {
        InputAction = InputActionObjForName->GetFName();
        InputActionObjForName = nullptr;
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - InputActionObjForName is nullptr for ability '{1}'. Attempting to retrieve from CDO.", FString(__FUNCTION__), AbilityTag.ToString());

        if (const UBaseAbility* CDO = Cast<const UBaseAbility>(GetClass()->GetDefaultObject()))
        {
            if (CDO->InputActionObjForName)
            {
                InputAction = CDO->InputActionObjForName->GetFName();
                InputActionObjForName = nullptr;

                UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Successfully retrieved InputAction '{1}' from CDO for ability '{2}'.", FString(__FUNCTION__), *InputAction.ToString(), AbilityTag.ToString());
                
            }
        }      
    }
}

void UBaseAbility::Activate() const
{
    if (!IsRunningDedicatedServer())
    {
          UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Not running on Client, skipping ability activation.", FString(__FUNCTION__));
        return;
    }

    if(!HasOwnerRequiredWeapon())
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Owner does not have required weapon to activate ability '{1}'.", FString(__FUNCTION__), AbilityTag.ToString());
        return;
    }

    OwnerAbilityComponent->OnAbilityActivated.Broadcast(AbilityTag); /*Server*/

    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Ability '{1}' can be activated.", FString(__FUNCTION__), AbilityTag.ToString());
}

void UBaseAbility::HandleInput(const ETriggerEvent TriggerEvent)
{
    UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Handling input event: {1}", FString(__FUNCTION__), StaticEnum<ETriggerEvent>()->GetValueAsString(TriggerEvent));

    switch (TriggerEvent)
    {
    case ETriggerEvent::None:
        UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Received ETriggerEvent::None, no action taken.", FString(__FUNCTION__));
        break;

    case ETriggerEvent::Started:
        OnInputStarted();
        break;

    case ETriggerEvent::Triggered:
        OnInputTriggered();
        break;

    case ETriggerEvent::Ongoing:
        OnInputOngoing();
        break;

    case ETriggerEvent::Canceled:
        OnInputCanceled();
        break;

    case ETriggerEvent::Completed:
        OnInputCompleted();
        break;

    default:
        break;
    }
}

FGameplayTag UBaseAbility::GetAbilityTag() const
{
    return AbilityTag;
}

FName UBaseAbility::GetInputActionName() const
{
    return InputAction;
}

bool UBaseAbility::HasOwnerRequiredWeapon() const
{
    auto WeaponComp = OwnerAbilityComponent->GetOwner()->FindComponentByClass<UWeaponComponent>();
    if (!WeaponComp)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Owner does not have a WeaponComponent.", FString(__FUNCTION__));
        return false;
    }

    bool bHasRequiredWeapon{false};
    AWeaponBase* CurrentWeapon = WeaponComp->GetCurrentWeapon();
    AWeaponThrowable* CurrentThrowableItem = WeaponComp->GetCurrentThrowableItem();

    if (!CurrentWeapon && !CurrentThrowableItem)
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - Owner's current weapon or throwable item does not match required tags for ability '{1}'.", FString(__FUNCTION__), AbilityTag.ToString());
        return false;
    }
    else if (/*!CurrentWeapon &&*/ CurrentThrowableItem)
    {
        bHasRequiredWeapon = WeaponComp->GetCurrentThrowableItem()->GetWeaponTag().MatchesAny(RequiredWeaponTags);
    }
    else if (CurrentWeapon && !CurrentThrowableItem)
    {
        bHasRequiredWeapon = WeaponComp->GetCurrentWeapon()->GetWeaponTag().MatchesAny(RequiredWeaponTags);
    }
    else
    {
        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - hasn't been found matching case.", FString(__FUNCTION__));
    }

    return bHasRequiredWeapon;
}

void UBaseAbility::OnInputStarted_Implementation() {}
void UBaseAbility::OnInputTriggered_Implementation() {}
void UBaseAbility::OnInputOngoing_Implementation() {}
void UBaseAbility::OnInputCanceled_Implementation() {}

void UBaseAbility::OnInputCompleted_Implementation()
{
    Activate();
}
