// Copyright Epic Games, Inc. All Rights Reserved.


#include "Character/CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void ACustomPlayerController::AddInputMappingContext(UInputMappingContext* MappingContext, int32 Priority)
{   
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayer());

    if (!MappingContext || !LocalPlayer)
    {
        return;
    }


    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        Subsystem->AddMappingContext(MappingContext, Priority);
    }

}