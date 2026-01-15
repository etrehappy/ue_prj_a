
#include "GeneralGameMode.h"
#include "Character/CustomPlayerController.h"
#include "ProjectALog.h"


void AGeneralGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACustomPlayerController* PlayerControllerP = Cast<ACustomPlayerController>(NewPlayer);
	if (!PlayerControllerP)
	{
		UE_LOGFMT(LogProjectA, Warning, "{0} - PlayerController is not ACustomPlayerController", FString(__FUNCTION__));     
		return;
	}

	PlayerControllerP->Client_OnConnected(GetMapIdentifier());
}

EServerWorldType AGeneralGameMode::GetMapIdentifier() const
{
	return EServerWorldType::None;
}
