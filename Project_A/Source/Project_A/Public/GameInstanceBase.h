

#pragma once

#include "CoreMinimal.h"
#include "GeneralGameMode.h"
#include "Blueprint/UserWidget.h"

#include "GameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API UGameInstanceBase : public UGameInstance
{
    GENERATED_BODY()

public:
    UGameInstanceBase() = default;
    virtual ~UGameInstanceBase() = default;

    virtual void Init() override;
    virtual void Shutdown() override;    
    void HideCurrentFullScreenWidget();

    /*!
    * @brief Executed when the connection to the server is successful
    */
    void OnConnectedSuccessfully(EServerWorldType World);

private:
    virtual void OnStart() override;
    void OnPostLoadMap(UWorld* LoadedWorld);
    void ShowMouse();
    void HideMouse();

public:
    UFUNCTION(BlueprintCallable)
    void ConnectToHubServer();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ShowLoadingScreen();  

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ShowErrorScreen(FString& ErrorMessage);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ShowMainMenu();


protected:
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
    class UUserWidget* CurrentFullScreenWidget;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
    FString PendingErrorMessage;
};
