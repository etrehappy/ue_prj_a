#pragma once

#include "CoreMinimal.h"

namespace NetSet
{
	inline constexpr TCHAR HubServerAddress[] = TEXT("127.0.0.1:7777");
	//inline constexpr TCHAR MainWorldServerAddress[] = TEXT("127.0.0.1:7778"); // seted via command line argument

	inline constexpr TCHAR HubHeartbeatAddress[] = TEXT("127.0.0.1");
	inline constexpr int32 HubHeartbeatPort = 7780;

} // namespace NetSet

UENUM(BlueprintType)
enum class EServerWorldType : uint8
{
	None			UMETA(DisplayName = "None")
	, Test			UMETA(DisplayName = "Test")
	, ClientStart	UMETA(DisplayName = "ClientStart")
	, Hub			UMETA(DisplayName = "Hub")
	, MainWorld		UMETA(DisplayName = "MainWorld")
};