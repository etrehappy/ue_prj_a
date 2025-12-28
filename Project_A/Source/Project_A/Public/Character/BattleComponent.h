

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "BattleComponent.generated.h"


class ANetPlayerCharacter;

UCLASS( ClassGroup=(CustomBattle), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UBattleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBattleComponent();
	virtual ~UBattleComponent() = default;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BindActions(UEnhancedInputComponent* EIC);
    void EquipWeapon(const FInputActionValue& Value);

protected:	
	virtual void BeginPlay() override;

private:
	void Initialize();

	ANetPlayerCharacter* Owner{};

public:	
	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InputActionEquipWeapon{};
	
};
