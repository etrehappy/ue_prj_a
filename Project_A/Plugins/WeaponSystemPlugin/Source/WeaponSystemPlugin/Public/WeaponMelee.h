/*****************************************************************//**
 * \file   WeaponMelee.h
 * \brief  A weapon is created using the WeaponFactory in Blueprints with the WeaponDataAsset.
 * 
 * \date   January 2026
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.h"

#include "WeaponMelee.generated.h"

/**
 * @class AWeaponMelee
 * @todo Update static member variables.
 */
UCLASS()
class WEAPONSYSTEMPLUGIN_API AWeaponMelee : public AWeaponBase
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:	
	AWeaponMelee();
	~AWeaponMelee() = default;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/**
	 * @brief Update AWeaponBase::bIsWeaponAttacking
	 */
	virtual void Client_StartAttack_Implementation() override;

	/**
	 * @brief Update AWeaponBase::bIsWeaponAttacking
	 */
	virtual void Client_StopAttack_Implementation() override;

private:
	/**
	 * @brief Sets up initial tip and base positions after BeginPlay.
	 * 
	 */
	void AfterBeginPlay();

	/**
	 * @brief Client function.
	 * 
	 * Updates weapon swing positions and sends them to the server for hit detection.
	 */
	void TickAttack(float DeltaTime);


						/* === C++ member variables === */
private:
	/**
	 * @brief The tip should be placed in the socket of a mesh.
	 */
	FVector LastTipPosition{};

	/**
	 * @brief The base should be placed in the socket of a mesh.
	 */
	FVector LastBasePosition{};

	/**
	 * @see Server_SendSwingPositions
	 * @todo It should be configurable via DataAsset or Blueprints.
	 */
	const float MaxClientPositionDelta {500.f};

	/**
	 * @brief Blade radius for hit detection.
	 * @todo It should be configurable via DataAsset or Blueprints.
	 */
	const float WeaponRadius {5.f};






	/* ================================================================
	 * Per-attack server-side guard to ensure a single successful hit
	 * is applied for one weapon attack cycle (one press/animation).
	 * The weapon itself does not track input count; the server
	 * simply allows only one damage application per attack start.
	 * ================================================================ */
protected:
	/**
	 * @brief Server-only flag: was damage already applied during the current attack cycle.
	 */
	bool bHasAppliedDamageThisAttack{false};

	/**
	 * @brief Attempt to consume the attack hit on the server.
	 * @return true if damage may be applied now (and marks it consumed), false if already consumed.
	 */
	bool ConsumeAttackHit();

	/**
	 * @brief Reset per-attack consumed flag (called when an attack starts).
	 */
	void ResetAttackHit();

	virtual void Server_SetIsWeaponAttacking_Implementation(bool bNewIsWeaponAttacking) override;







	
						/* === Unreal Engine UFUNCTION === */
protected:
	/**
	 * @brief The client sends the weapon swing positions to the server for verification and application of damage.
	 *  If the client sends coordinates that exceed the permitted threshold (MaxClientPositionDelta), the server will reject the data. 
	 * 
	 * @param[in] LastBase position of the weapon base in the last frame.
	 * @param[in] BaseNow position of the weapon base in the current frame.
	 * @param[in] LastTip position of the weapon tip in the last frame.
	 * @param[in] TipNow position of the weapon tip in the current frame.
	 * @param[in] Radius the radius of the weapon's hit detection sphere.
	 */
	UFUNCTION(Server, Unreliable)
	void Server_SendSwingPositions(const FVector LastBase, const FVector BaseNow, const FVector LastTip, const FVector TipNow, float Radius);

	/**
	 * @brief Editor-only function.
	 */
	UFUNCTION(Client, Unreliable)
	void Client_DrawDebug(FVector LastBase, FVector BaseNow, FVector LastTip, FVector TipNow, float Radius);	
	
};

//void TickAttack(float DeltaTime);