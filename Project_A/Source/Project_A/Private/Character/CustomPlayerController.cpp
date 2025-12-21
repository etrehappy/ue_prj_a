// Copyright Epic Games, Inc. All Rights Reserved.


#include "Character/CustomPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "GameInstanceBase.h"
#include "HubGameMode.h"

#include "ProjectALog.h"


void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!IsLocalController() ) { return; }

    // Add Input Mapping Contexts
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {

        for (const FInputMappingContextWithPriority& Item : InputMappingContext)
        {
            if (!Item.MappingContext)
            {
                UE_LOGFMT(LogProjectA, Warning, "{0} — DefaultMappingContexts contains a null entry", FString(__FUNCTION__));              
                continue;
            }
            
            Subsystem->AddMappingContext(Item.MappingContext, Item.Priority);
        }
    }
}

void ACustomPlayerController::OnPossess(APawn* APawn)
{
    Super::OnPossess(APawn);

    UE_LOGFMT(LogProjectA, Log, "{0} is executed: PlayerControllerName - {1}, PawnName - {2} ", FString(__FUNCTION__), *GetNameSafe(this), *GetNameSafe(APawn));
}

void ACustomPlayerController::OnUnPossess()
{
    Super::OnUnPossess();

}

void ACustomPlayerController::Client_OnConnected_Implementation(EServerWorldType World)
{    
    UE_LOGFMT(LogProjectA, Log, "{0} — Connected to the server successfully", FString(__FUNCTION__));

    UGameInstanceBase* GameInstanceP = GetGameInstance<UGameInstanceBase>();
    if (!GameInstanceP)
    {   
        UE_LOGFMT(LogProjectA, Warning, "{0} —  GameInstance is not found", FString(__FUNCTION__));
        return;
    };

    GameInstanceP->OnConnectedSuccessfully(World);
}

void ACustomPlayerController::Server_RequestEnterToWorld_Implementation()
{   
    AHubGameMode* GameModeP = GetWorld()->GetAuthGameMode<AHubGameMode>();
    if (!GameModeP) 
    {       
        UE_LOGFMT(LogProjectA, Warning, "{0} —  GameModeP is not found", FString(__FUNCTION__));
        return;
    };    

    GameModeP->EnterToWorld(this);
}




///////////////////////////////////////////////////////////////////////////////
//AutoTests

#if WITH_DEV_AUTOMATION_TESTS

const TArray<FInputMappingContextWithPriority>& ACustomPlayerController::AutoTestGetInputMappingContext() const
{    
    return InputMappingContext;
}

#endif //WITH_DEV_AUTOMATION_TESTS