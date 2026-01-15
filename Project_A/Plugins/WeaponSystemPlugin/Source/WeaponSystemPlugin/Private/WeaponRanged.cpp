


#include "WeaponRanged.h"
#include "GameFramework/Character.h"
#include "Animation/AnimLayerInterface.h"
#include "WeaponDataAsset.h"
#include "WeaponPluginLog.h"

// Sets default values
AWeaponRanged::AWeaponRanged()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AWeaponRanged::InitialiseFromData(UWeaponDataAsset* Data)
{
    Super::InitialiseFromData(Data);

	/*Client_LinkAnimClassLayers(Data->WeaponAnimBPClass);*/
}

// Called when the game starts or when spawned
void AWeaponRanged::BeginPlay()
{
	Super::BeginPlay();

	

	
}

void AWeaponRanged::Client_StartAttack_Implementation()
{
}

void AWeaponRanged::Client_StopAttack_Implementation()
{
}

//void AWeaponRanged::Client_LinkAnimClassLayers_Implementation(TSubclassOf<UAnimInstance> InWeaponAnimBPClass)
//{
//	SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComponent);
//	if (!SkeletalMeshComponent)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Error, "{0} - SkeletalMeshComponent is null", FString(__FUNCTION__));
//		return;
//	}
//
//    if (!InWeaponAnimBPClass)
//	{
//        UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - InWeaponAnimBPClass is null", FString(__FUNCTION__));
//	}
//
//	SkeletalMeshComponent->SetAnimInstanceClass(InWeaponAnimBPClass);
//    SkeletalMeshComponent->SetLeaderPoseComponent(nullptr); // Ensure no leader pose is set
//
//	if (!OwningCharacter)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Warning, "{0} - OwningCharacter was null", FString(__FUNCTION__));
//        OwningCharacter = GetOwner<ACharacter>();        
//	}
//
//	if (!OwningCharacter->GetMesh())
//	{
//		UE_LOGFMT(LogWeaponPlugin, Error, "{0} - OwningCharacter Mesh is null", FString(__FUNCTION__));
//		return;
//	}
//
//	UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
//	if (!AnimInstance)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Error, "{0} - AnimInstance is null", FString(__FUNCTION__));
//		return;
//	}
//
//	TSubclassOf<UAnimInstance> AnimClass = AnimInstance->GetClass();
//	if (!AnimClass)
//	{
//		UE_LOGFMT(LogWeaponPlugin, Error, "{0} - AnimClass is null", FString(__FUNCTION__));
//		return;
//    }
//	SkeletalMeshComponent->LinkAnimClassLayers(AnimClass);
//
//	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - Linked AnimClass layers to SkeletalMeshComponent", FString(__FUNCTION__));
//}

// Called every frame
void AWeaponRanged::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


