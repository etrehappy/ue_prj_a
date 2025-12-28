

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

//#include "WeaponFactory.h"

#include "WeaponBase.generated.h"

UCLASS()
class WEAPONSYSTEMPLUGIN_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:		
	AWeaponBase();
	virtual ~AWeaponBase() = default;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeFromData(class UWeaponDataAsset* Data);
	virtual void Attack() {};
	const FGameplayTag& GetWeaponTag() const;

protected:	
	virtual void BeginPlay() override;

public:
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_SetCollision(ECollisionEnabled::Type CollisionEnabled, ECollisionResponse CollisionResponse);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Weapon")
	FGameplayTag WeaponTag{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* Mesh{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon")
	float Damage{10.f};
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//float AttackRate{};

	//UFUNCTION(Server, Reliable)
	//void Server_InitializeFromData(UWeaponDataAsset* Data);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetStaticMesh(UStaticMesh* NewMesh);
};
