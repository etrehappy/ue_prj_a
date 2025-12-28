

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "WeaponBase.h"


#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent) )
class WEAPONSYSTEMPLUGIN_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const bool IsWeaponCurrentlyEquiped() const; 

protected:
	virtual void BeginPlay() override;
	
public:
	//UFUNCTION(BlueprintCallable)
	//AWeaponBase* EquipWeaponbyClass(TSubclassOf<AWeaponBase> WeaponClass);

	UFUNCTION(BlueprintCallable)
	void EquipWeaponbyTag(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform& SpawnTransform, USkeletalMeshComponent* AttachTo = nullptr, bool bIsCollisionDesabled = true);
	
	UFUNCTION(BlueprintCallable)
	virtual void UnequipWeapon();

	UFUNCTION(BlueprintCallable)
	void Attack() {};

	UFUNCTION(BlueprintCallable)
	void Reload() {};

	UFUNCTION(Server, Reliable, BlueprintCallable = false)
	void Server_UnequipWeapon();	

protected:

	UFUNCTION(Server, Reliable)
	void Server_EquipWeaponByTag(const FGameplayTag WeaponTag, const FName AttachSocketName, const FTransform SpawnTransform, ACharacter* AttachToCharacter, bool bIsCollisionDesabled = true);

	UFUNCTION()
	void OnRep_CurrentWeapon();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* CurrentWeapon{};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool IsWeaponEquiped{false};
	
};
