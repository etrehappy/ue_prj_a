#pragma once

#include "CoreMinimal.h"
#include "NpcTypes.generated.h"

UENUM(BlueprintType)
enum class ENpcFaction : uint8
{
	Friendly		UMETA(DisplayName = "Friendly"),
	Enemy			UMETA(DisplayName = "Enemy"),
	EnemyGoblin		UMETA(DisplayName = "Enemy Goblin")
};
