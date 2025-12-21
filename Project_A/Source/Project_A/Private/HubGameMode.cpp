
#include "HubGameMode.h"
#include "Character/CustomPlayerController.h"

#include "NetworkSettings.h"
#include "ProjectALog.h"

AHubGameMode::AHubGameMode()
{
    PlayerControllerClass = ACustomPlayerController::StaticClass();
    //DefaultPawnClass = AMyCharacter::StaticClass(); 
    // HUDClass = AMyHUD::StaticClass();    
    // GameStateClass = AMyGameState::StaticClass();        
    // PlayerStateClass = AMyPlayerState::StaticClass();
    // SpectatorClass = AMySpectatorPawn::StaticClass();
}

void AHubGameMode::EnterToWorld(APlayerController* PlayerControllerP)
{
    if (!PlayerControllerP)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — PlayerController is empty", FString(__FUNCTION__));   
        return;
    }

    PlayerControllerP->ClientTravel(NetSet::MainWorldServerAddress, ETravelType::TRAVEL_Absolute);
}

EServerWorldType AHubGameMode::GetMapIdentifier() const
{
    return EServerWorldType::Hub;
}
