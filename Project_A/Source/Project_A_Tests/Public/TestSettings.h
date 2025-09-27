#pragma once
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
namespace SetsForTests
{
    static const FString TestMapThirdPerson = TEXT("Lvl_ThirdPerson");
    static const FString TestMapThirdPersonPath = TEXT("/Game/ThirdPerson/Lvl_ThirdPerson");

    static const TCHAR* BpClassPath = TEXT("/Game/Project_A_Root/Character/Player/BP_PlayerCharacter.BP_PlayerCharacter_C");


} // namespace SetsForTests
#endif // WITH_DEV_AUTOMATION_TESTS