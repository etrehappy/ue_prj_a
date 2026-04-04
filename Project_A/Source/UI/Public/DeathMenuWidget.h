#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "DeathMenuWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathMenuRespawnRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathMenuExitRequested);

/**
 * @class UDeathMenuWidget
 * @brief Widget displayed when the player dies, providing options to respawn or exit.
 */
UCLASS()
class UI_API UDeathMenuWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	UDeathMenuWidget() = default;
	virtual ~UDeathMenuWidget() = default;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	UFUNCTION()
	void HandleRespawnClicked();

	UFUNCTION()
	void HandleExitClicked();

public:
	UPROPERTY(BlueprintAssignable, Category = "DeathMenu")
	FOnDeathMenuRespawnRequested OnRespawnRequested{};

	UPROPERTY(BlueprintAssignable, Category = "DeathMenu")
	FOnDeathMenuExitRequested OnExitRequested{};

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> RespawnButton{};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitButton{};
};