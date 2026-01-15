


#include "Animation/UAnimAttackNotifies.h"
#include "Character/NetPlayerCharacter.h"
#include "Character/CombatComponent.h"
#include "ProjectALog.h"


namespace
{
	UCombatComponent* CheckCharacter(AActor* Owner)
	{        
	   
		if (!Owner)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Owner = nullptr", FString(__FUNCTION__));
			return nullptr;
		}

		ANetPlayerCharacter* Character = Cast<ANetPlayerCharacter>(Owner);
		if (!Character)
		{
			UE_LOGFMT(LogProjectA, Warning, "{0} - Character = nullptr", FString(__FUNCTION__));
			return nullptr;
		}

		UCombatComponent* CombatComponent{};
		CombatComponent = Character->FindComponentByClass<UCombatComponent>();           

		return CombatComponent;
	}
}

void UAttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - Notify called", FString(__FUNCTION__));

	UCombatComponent* CombatComponent = ::CheckCharacter(MeshComp->GetOwner());
	if (!CombatComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - CombatComponent = nullptr", FString(__FUNCTION__));
		return;
	}

	CombatComponent->OnAttackAnimationFinished();

}

void UAnimNotify_SpawnMagicProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - Notify called", FString(__FUNCTION__));

	UCombatComponent* CombatComponent = ::CheckCharacter(MeshComp->GetOwner());
	if (!CombatComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - CombatComponent = nullptr", FString(__FUNCTION__));
		return;
	}

	CombatComponent->OnSpawnProjectile();

	//UE_LOGFMT(LogProjectA, Warning, "{0} - Notify finished", FString(__FUNCTION__));
}


void UAnimNotify_SpawnBombProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOGFMT(LogProjectA, Log, "{0} - Notify called", FString(__FUNCTION__));

	UCombatComponent* CombatComponent = ::CheckCharacter(MeshComp->GetOwner());
	if (!CombatComponent)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - CombatComponent = nullptr", FString(__FUNCTION__));
		return;
	}

	CombatComponent->OnSpawnBombProjectile();

	//UE_LOGFMT(LogProjectA, Warning, "{0} - Notify finished", FString(__FUNCTION__));
}
