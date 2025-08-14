//Common headers
// 
#include "Editor/EditorEngine.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
//#include "Misc/AutomationTest.h"

//#include "Engine.h"
//#include "EngineUtils.h"
//#include "FileHelpers.h" //(UEditorLoadingAndSavingUtils , FEditorFileUtils)

//Project_A headers
#include "TestSettings.h"
#include "ActorForTestNull.h"


#if WITH_DEV_AUTOMATION_TESTS



/// 
/// TESTS
/// 

/**
 * @brief Just shows Contexts
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShowWorldContextsTest, "TrainingTests.Show WorldContexts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FShowWorldContextsTest::RunTest(const FString& Parameters)
{
	AddForTests::LogAllWorldContexts();

	return true;
}

/**
 * @brief Just checks World type 
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShowWorldTypeTest, "TrainingTests. Show WorldType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FShowWorldTypeTest::RunTest(const FString& Parameters)
{
	UWorld* World{};
	World = GEditor->GetEditorWorldContext().World();
	AddForTests::LogWorldType(World);
	
	TestNotNull(TEXT("World was found"), World);

	return true;
}


/**
 * @brief Uses the currently active map in the Level Editor.
 * @return true — if there is a Blueprint actor on the map that has a target.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGetBpActorPropertiesFromEditorTest, "TrainingTests.InEditor.GetBpActorProperties",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGetBpActorPropertiesFromEditorTest::RunTest(const FString& Parameters)
{
	//1. Arrange

	UWorld* World{};
	AActorForTestNull* TestActor{};
	FVector TargetActorLocation{};
	const TCHAR* BpClassPath = TEXT("/Game/Project_A_Root/Test/BP_TestNullActor.BP_TestNullActor_C");
	TSubclassOf<AActor> BpClass = StaticLoadClass(AActorForTestNull::StaticClass(), nullptr, BpClassPath);	

	//Takes map from the Level Editor.
	World = GEditor->GetEditorWorldContext().World();

	if (!World)
	{
		AddError(TEXT("World is null"));
		return false;
	}

	auto FoundActor = UGameplayStatics::GetActorOfClass(World, BpClass);
	TestActor = Cast<AActorForTestNull>(FoundActor);
	if (!TestNotNull(TEXT("TestActor is found"), TestActor))
	{
		return false;
	}

	TargetActorLocation = TestActor->GetTargetActorLocation();

	//2. Act
	
	//3. Assert

	TestNotEqual(TEXT("Actor's location not equal null"), TargetActorLocation, FVector::ZeroVector);

	return true;
}


DEFINE_LATENT_AUTOMATION_COMMAND(FNUTWaitForUnitTests);




/**
 * @brief Open a map in the PIE-mod.
 * @return true — if the map is opened.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOpenLevelTest, "TrainingTests.PIE.OpenLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FOpenLevelTest::RunTest(const FString& Parameters)
{
	//1. Arrange
	const FString MapPath = SetsForTests::TestMapThirdPersonPath;

	//2. Act
	ADD_LATENT_AUTOMATION_COMMAND(FEditorLoadMap(MapPath));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));

	ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));

	ADD_LATENT_AUTOMATION_COMMAND(FDelayedFunctionLatentCommand([this, MapPath]() -> bool
		{			
			UWorld* World = GEditor->GetPIEWorldContext()->World();;
			if (TestNotNull(TEXT("World must be valid after map load"), World))
			{
				return true;
			}

			const FString LoadedMap = World->GetMapName();
			const TCHAR* TestDescription = *FString::Printf(TEXT("Loaded map name contains '%s'"), *MapPath);
			const bool TestValue = LoadedMap.Contains(FPaths::GetBaseFilename(MapPath));

	//3. Assert
			TestTrue(TestDescription, TestValue);

			return true;

		})
	);

	return true;
}






#endif // WITH_DEV_AUTOMATION_TESTS