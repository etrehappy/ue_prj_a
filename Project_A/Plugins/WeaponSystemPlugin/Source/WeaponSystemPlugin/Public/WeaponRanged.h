/*****************************************************************//**
 * \file   WeaponRanged.h
 * \brief  This weapon isn't ready and should not be used.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.h"

#include "WeaponRanged.generated.h"



/*
* @brief: This class isn't ready and should not be used.
*/
UCLASS()
class WEAPONSYSTEMPLUGIN_API AWeaponRanged : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponRanged();
	virtual void InitialiseFromData(class UWeaponDataAsset* Data) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UFUNCTION(Client, Reliable)
	virtual void Client_StartAttack_Implementation() override;/*TODO Check Net*/

	//UFUNCTION(Client, Reliable)
	virtual void Client_StopAttack_Implementation() override;/*TODO Check Net*/

	//UFUNCTION(Client, Reliable)
	//void Client_LinkAnimClassLayers(TSubclassOf<UAnimInstance> InWeaponAnimBPClass);/*TODO Check Net*/

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent{};



	
};
