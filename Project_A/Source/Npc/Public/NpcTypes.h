#pragma once

#include "CoreMinimal.h"
#include "NpcTypes.generated.h"

UENUM(BlueprintType)
enum class ENpcFaction : uint8
{
	Friendly	UMETA(DisplayName = "Friendly"),
	Enemy		UMETA(DisplayName = "Enemy")
};

//UENUM(BlueprintType)
//enum class ENpcBehaviorType : uint8
//{
//	Patrol		UMETA(DisplayName = "Patrol"),
//	Chaser		UMETA(DisplayName = "Chaser"),
//	Observer	UMETA(DisplayName = "Observer")
//};