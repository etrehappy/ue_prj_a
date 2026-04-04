/*****************************************************************//**
 * \file   HealthWidget.h
 * \brief  This is the health widget class that extends the base widget functionality.
 * 
 * \date   April 2026
 *********************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "HealthWidget.generated.h"

class UHealthComponent;
class UProgressBar;
class UTextBlock;

/**
 * @class UHealthWidget
 * @brief This widget displays the health of a character.
 */
UCLASS()
class UI_API UHealthWidget : public UBaseWidget
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UHealthWidget() = default;
	virtual ~UHealthWidget() = default;

private:
	TWeakObjectPtr<UHealthComponent> HealthComponent{};

						/* === Unreal Engine UFUNCTION === */
public:
	UFUNCTION()
	void InitialiseWithHealthComponent(UHealthComponent* InHealthComponent);

private:
	UFUNCTION()
	void HandleHealthChanged(float CurrentHealthValue, float MaxHealthValue);


						/* === Unreal Engine UPROPERTY === */
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HpProgressBar{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HpText{};
};