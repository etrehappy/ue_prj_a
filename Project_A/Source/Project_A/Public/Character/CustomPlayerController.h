#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GeneralGameMode.h"

#include "CustomPlayerController.generated.h"

class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputMappingContextWithPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* MappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;
};




UCLASS()
class PROJECT_A_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACustomPlayerController() = default;
    virtual ~ACustomPlayerController() = default;

protected:
	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestEnterToWorld();

	UFUNCTION(Client, Reliable)
	void Client_OnConnected(EServerWorldType World);

protected:

	/**
	 * @brief Which keys or axes trigger which actions.
	 * @note This field must be set before running the game.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<FInputMappingContextWithPriority> InputMappingContext{};



#if WITH_DEV_AUTOMATION_TESTS
public:
	const TArray<FInputMappingContextWithPriority>& AutoTestGetInputMappingContext() const;
#endif // WITH_DEV_AUTOMATION_TESTS
	

};
