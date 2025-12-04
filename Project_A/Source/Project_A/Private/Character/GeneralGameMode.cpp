


#include "Character/GeneralGameMode.h"

AGeneralGameMode::AGeneralGameMode()
{
    //DefaultPawnClass = AMyCharacter::StaticClass();
       
    PlayerControllerClass = ACustomPlayerController::StaticClass();
        
    // HUDClass = AMyHUD::StaticClass();
    
    // GameStateClass = AMyGameState::StaticClass();
        
    // PlayerStateClass = AMyPlayerState::StaticClass();

    // SpectatorClass = AMySpectatorPawn::StaticClass();
}