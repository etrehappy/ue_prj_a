

#pragma once
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace SetsForTests 
{
    static const FString TestMapThirdPerson = TEXT("Lvl_ThirdPerson");
    static const FString TestMapThirdPersonPath = TEXT("/Game/ThirdPerson/Lvl_ThirdPerson");


} // namespace SetsForTests


/// 
/// Additional functions
/// 
namespace AddForTests
{
	static void LogWorldType(UWorld* World)
	{
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("World == nullptr"));
			return;
		}

		switch (World->WorldType)
		{
		case EWorldType::Editor:
			UE_LOG(LogTemp, Log, TEXT("WorldType: Editor"));
			break;

		case EWorldType::PIE:
			UE_LOG(LogTemp, Log, TEXT("WorldType: PIE or Simulate"));
			break;

		case EWorldType::Game:
			UE_LOG(LogTemp, Log, TEXT("WorldType: Game or Standalone)"));
			break;

		case EWorldType::GamePreview:
			UE_LOG(LogTemp, Log, TEXT("WorldType: GamePreview"));
			break;

		case EWorldType::EditorPreview:
			UE_LOG(LogTemp, Log, TEXT("WorldType: EditorPreview"));
			break;

		case EWorldType::GameRPC:
			UE_LOG(LogTemp, Log, TEXT("WorldType: GameRPC"));
			break;

		case EWorldType::Inactive:
			UE_LOG(LogTemp, Log, TEXT("WorldType: Inactive"));
			break;

		default:
			UE_LOG(LogTemp, Log, TEXT("WorldType: %d (another)"), (int32)World->WorldType);
			break;
		}
	}


	static void LogAllWorldContexts()
	{
		if (!GEngine)
		{
			UE_LOG(LogTemp, Warning, TEXT("GEngine is not initialised"));
			return;
		}

		//const TIndirectArray<FWorldContext>& Contexts = GEngine->GetWorldContexts();
		const TIndirectArray<FWorldContext>& Contexts = GEditor->GetWorldContexts();
		UE_LOG(LogTemp, Log, TEXT("=== Found %d world contexts ==="), Contexts.Num());

		for (const FWorldContext& Ctx : Contexts)
		{
			UWorld* World = Ctx.World();
			FString WorldName = World ? World->GetName() : TEXT("None");
			int32 TypeInt = static_cast<int32>(Ctx.WorldType);


			UE_LOG(LogTemp, Log, TEXT("ContextHandle: %s"), *Ctx.ContextHandle.ToString());
			UE_LOG(LogTemp, Log, TEXT("  World pointer: %p"), World);
			UE_LOG(LogTemp, Log, TEXT("  World name: %s"), *WorldName);
			LogWorldType(World);
			/*	UE_LOG(LogTemp, Log, TEXT("  WorldType (%d): %s"),
					TypeInt, *UEnum::GetValueAsString(Ctx.WorldType));*/
			UE_LOG(LogTemp, Log, TEXT("-----------------------------------"));
		}
	}
} // namespace AddForTests

#endif // WITH_DEV_AUTOMATION_TESTS