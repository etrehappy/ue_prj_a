//Common headers
#include "Editor/EditorEngine.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

//Project_A headers
#include "TestSettings.h"
#include "ActorForTestNull.h"


#if WITH_DEV_AUTOMATION_TESTS

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

#endif // WITH_DEV_AUTOMATION_TESTS