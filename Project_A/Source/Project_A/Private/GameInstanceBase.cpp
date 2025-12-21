#include "GameInstanceBase.h"

#include "Kismet/GameplayStatics.h"

#include "NetworkSettings.h"
#include "ProjectALog.h"

void UGameInstanceBase::Init()
{
    Super::Init();     
    UE_LOGFMT(LogProjectA, Log,
        "\n"
        "\n##################################################################################\n"
        "# {0} — GameInstance Init\n"
        "##################################################################################\n",
        FString(__FUNCTION__));
     
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject( this,
        &UGameInstanceBase::OnPostLoadMap
    );  
}

void UGameInstanceBase::Shutdown()
{        
    UE_LOGFMT(LogProjectA, Log,
        "\n"
        "\n##################################################################################\n"
        "# {0} — GameInstance Shutdown\n"
        "##################################################################################\n",
        FString(__FUNCTION__));

    Super::Shutdown();
}

void UGameInstanceBase::OnStart()
{
    Super::OnStart();

    if (IsRunningDedicatedServer())
    { 
        UE_LOGFMT(LogProjectA, Log, "{0} — Running Dedicated Server, skipping this", FString(__FUNCTION__));
        return;
    }   

    ShowLoadingScreen();
    ConnectToHubServer();
}

void UGameInstanceBase::OnPostLoadMap(UWorld* LoadedWorld)
{
    UE_LOGFMT(LogProjectA, Log, "{0} — Map loaded: {1}", FString(__FUNCTION__), *LoadedWorld->GetName() );
    
    if (IsRunningDedicatedServer())
    {
        UE_LOGFMT(LogProjectA, Log, "{0} — Running Dedicated Server, skipping this", FString(__FUNCTION__));
        return;
    }

    if (!PendingErrorMessage.IsEmpty())
    {
        ShowErrorScreen(PendingErrorMessage);
        PendingErrorMessage.Empty();
        return;
    }

    HideCurrentFullScreenWidget();
}

void UGameInstanceBase::ShowMouse()
{    
    APlayerController* PlayerControllerP = GetWorld()->GetFirstPlayerController();
    if (!PlayerControllerP)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — PlayerController is not found", FString(__FUNCTION__));
        return;
    }

    PlayerControllerP->SetShowMouseCursor(true);
    PlayerControllerP->SetInputMode(FInputModeUIOnly());
}

void UGameInstanceBase::HideMouse()
{
    APlayerController* PlayerControllerP = GetWorld()->GetFirstPlayerController();
    if (!PlayerControllerP)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — PlayerController is not found", FString(__FUNCTION__));
        return;
    }
    
    PlayerControllerP->SetShowMouseCursor(false);
    PlayerControllerP->SetInputMode(FInputModeGameOnly());
}

void UGameInstanceBase::OnConnectedSuccessfully(EServerWorldType World)
{
    if (IsRunningDedicatedServer())
    {
        UE_LOGFMT(LogProjectA, Log, "{0} — Running Dedicated Server, skipping this", FString(__FUNCTION__));
        return;
    }

    HideCurrentFullScreenWidget();

    switch (World)
    {
    case EServerWorldType::None:
        UE_LOGFMT(LogProjectA, Warning, "{0} — EServerWorldType::None", FString(__FUNCTION__) );   
        break;

    case EServerWorldType::Test:
        UE_LOGFMT(LogProjectA, Log, "{0} — EServerWorldType::Test", FString(__FUNCTION__));      
        break;

    case EServerWorldType::Hub:
        UE_LOGFMT(LogProjectA, Log, "{0} — EServerWorldType::Hub", FString(__FUNCTION__));
        ShowMainMenu();
        ShowMouse();
        break;

    case EServerWorldType::MainWorld:
        UE_LOGFMT(LogProjectA, Log, "{0} — EServerWorldType::MainWorld", FString(__FUNCTION__));
        break;

    default:
        UE_LOGFMT(LogProjectA, Warning, "{0} — EServerWorldType is not set", FString(__FUNCTION__));
        break;
    }    
}


void UGameInstanceBase::ConnectToHubServer()
{
    if (IsRunningDedicatedServer())
    {
        UE_LOGFMT(LogProjectA, Log, "{0} — Running Dedicated Server, skipping this", FString(__FUNCTION__));
        return;
    }

    UE_LOGFMT(LogProjectA, Log, "{0} — Connecting to the server...", FString(__FUNCTION__));

    UGameplayStatics::OpenLevel(this, FName(NetSet::HubServerAddress), true);
}

void UGameInstanceBase::HideCurrentFullScreenWidget()
{
    if (!CurrentFullScreenWidget)
    {        
        UE_LOGFMT(LogProjectA, Log, "{0} — CurrentFullScreenWidget is empty", FString(__FUNCTION__));
        return;
    }

    HideMouse();
    CurrentFullScreenWidget->RemoveFromParent();
    CurrentFullScreenWidget = nullptr;

    UE_LOGFMT(LogProjectA, Log, "{0} — CurrentFullScreenWidget was set to nullptr", FString(__FUNCTION__));   

}
