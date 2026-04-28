/*****************************************************************//**
 * \file   NpcAttackLogicWeaponSweep.h
 * \brief  A simple solution for npc's melee attack if npc has a skeletal weapon mesh.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "NpcAttackLogicBase.h"
#include "Engine/EngineTypes.h"

#include "NpcAttackLogicWeaponSweep.generated.h"

class AActor;
class APawn;
class USkeletalMeshComponent;

/**
 * @class UNpcAttackLogicWeaponSweep
 * @brief Implements a weapon sweep attack logic for NPCs. It performs a sweep trace between the tip and base of a weapon mesh to detect hits during an attack.
 */
UCLASS(Blueprintable)
class NPC_API UNpcAttackLogicWeaponSweep : public UNpcAttackLogicBase
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	virtual void Initialize(UNpcBattleComponent* InBattleComponent) override;
	virtual bool CanStartAttack() const override;
	virtual void OnAttackStarted() override;
	virtual void OnAttackStopped() override;
	virtual bool OnDamageWindowStarted() override;
	virtual void TickDamageWindow(float DeltaTime) override;
	virtual bool WantsTick() const override;

private:
	void InitialiseWeaponMeshComponent();
	void ApplyHitsFromSweep(TArray<FHitResult>& Hits, const FVector& Start, const FVector& End, FCollisionQueryParams& Params, APawn* OwnerPawn);


						/* === C++ member variables === */
private:
	FVector LastTipPosition{FVector::ZeroVector};
	FVector LastBasePosition{FVector::ZeroVector};
	TSet<TObjectPtr<AActor>> HitActorsThisAttack{};
						

						/* === Unreal Engine UPROPERTY === */
protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponMeshComponentTag{"WeaponMesh"};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName TipSocketName{"TipSocket"};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName BaseSocketName{"BaseSocket"};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponTraceRadius{6.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> WeaponTraceChannel{ECC_Pawn};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent{};


};