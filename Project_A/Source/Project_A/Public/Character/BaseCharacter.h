/*****************************************************************//**
 * \file   BaseCharacter.h
 * \brief  Base class for all characters in the game.
 * 
 * \date   December 2025
 *********************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/**
 * @class ABaseCharacter
 * @brief This clas hasn't implementation yet. Used by ANetPlayerCharacter as a base class.
 * @see ANetPlayerCharacter
 */
UCLASS(Abstract)
class PROJECT_A_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	
	
};
