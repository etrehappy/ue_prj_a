


#include "WorldGameMode.h"
#include "Character/CustomPlayerController.h"

AWorldGameMode::AWorldGameMode()
{
	PlayerControllerClass = ACustomPlayerController::StaticClass();
}

EServerWorldType AWorldGameMode::GetMapIdentifier() const
{
	return EServerWorldType::MainWorld;
}
