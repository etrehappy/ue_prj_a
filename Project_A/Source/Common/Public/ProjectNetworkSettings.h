#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EServerWorldType : uint8
{
	None			UMETA(DisplayName = "None")
	, Test			UMETA(DisplayName = "Test")
	, ClientStart	UMETA(DisplayName = "ClientStart")
	, Hub			UMETA(DisplayName = "Hub")
	, Lobby			UMETA(DisplayName = "Lobby")
	, MainWorld		UMETA(DisplayName = "MainWorld")
};