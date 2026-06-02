

#pragma once

#include "CoreMinimal.h"
#include "GeneralGameMode.h"



#include "GameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API UGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

						/* === C++ member functions === */
public:
	UGameInstanceBase() = default;
	virtual ~UGameInstanceBase() = default;

	virtual void Init() override;
	virtual void Shutdown() override;  

	// Auth
	void SetAccountId(const FString& InAccountId) { AccountId = InAccountId; }
	FString GetAccountId() const { return AccountId; }
	void SetSessionToken(const FString& InToken) { SessionToken = InToken; }
	FString GetSessionToken() const { return SessionToken; }
	bool IsAuthenticated() const;
	void ClearSession();

private:
	virtual void OnStart() override;
	void OnPostLoadMap(UWorld* LoadedWorld);

	void HandleEngineNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleEngineTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

	/**
	 * @brief Temporary solution
	 * @todo Is it still needed
	 */
	void PreloadInventoryItemDefinitions();
	void OnInventoryItemDefinitionsLoaded();


						/* === Unreal Engine UFUNCTION === */
public:
	UFUNCTION(BlueprintCallable)
	void ExitToDesktop();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SaveNetStatusWidgetRef(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CleanNetStatusWidgetRef() { NetConnectionStatusWidget = nullptr; };
		
	/**
	 * @see PendingErrorMessage 
	 */
	UFUNCTION(BlueprintPure)
	FString GetPendingErrorMessage() const { return PendingErrorMessage; }


						/* === Unreal Engine UPROPERTY === */
protected:
	/**
	 * @brief It is used to save a widget from Garbage Collector.
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TObjectPtr<UUserWidget> NetConnectionStatusWidget{};

	/**
	 * @brief It is used to show error message on UI after travel failure or network failure happens.
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString PendingErrorMessage{};

private:
	/**
	 * @brief Unique identifier of the authenticated account. Persists across map travels for the lifetime of the game instance.
	 */
	FString AccountId{};

	/**
	 * @brief Used to validate requests on the server. Persists across map travels for the lifetime of the game instance.
	 */
	FString SessionToken{};
};