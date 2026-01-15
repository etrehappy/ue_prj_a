

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UAnimAttackNotifies.generated.h"


/**
 * @brief 
 * @todo UAttackFinished should doesn't know about UCombatComponent, consider using event dispatcher or interface.
 */
UCLASS()
class PROJECT_A_API UAttackFinished : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};


UCLASS()
class UAnimNotify_SpawnMagicProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

UCLASS()
class UAnimNotify_SpawnBombProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

