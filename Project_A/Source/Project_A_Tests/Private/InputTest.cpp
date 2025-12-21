#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"


//#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "TestSettings.h"
#include "Character/CustomLocomotionComponent.h"
#include "Character/CustomPlayerController.h"
#include "Character/NetPlayerCharacter.h"

#if WITH_DEV_AUTOMATION_TESTS



IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInputActionsAreSetTest, "Project_A.Character.Input.ActionsAreSet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInputActionsAreSetTest::RunTest(const FString& Parameters)
{
    //1. Arrange
    const FString MapPath = SetsForTests::TestMapThirdPersonPath;    

   
    ADD_LATENT_AUTOMATION_COMMAND(FEditorLoadMap(MapPath));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));

    ADD_LATENT_AUTOMATION_COMMAND(FDelayedFunctionLatentCommand([this]() -> bool
    {
        int counter = 0;
        UWorld* World = GEditor->GetPIEWorldContext()->World();

        if (!TestNotNull(TEXT("World is valid after PIE start"), World)) { return true; }

        TSubclassOf<AActor> BpClass = StaticLoadClass(ANetPlayerCharacter::StaticClass(), nullptr, SetsForTests::BpClassPath);
        if (!TestNotNull(TEXT("BP class loaded"), *BpClass)) { return true; }

        //2. Act
        const auto FoundActor = UGameplayStatics::GetActorOfClass(World, BpClass);
        const ANetPlayerCharacter* const TestActor = Cast<ANetPlayerCharacter>(FoundActor);
        if (!TestNotNull(TEXT("TestActor is found"), TestActor)) { return true; }

        UCustomLocomotionComponent* Locomotion = TestActor->FindComponentByClass<UCustomLocomotionComponent>();
        if (!Locomotion)
        {
            AddError(FString::Printf(TEXT("CustomLocomotionComponent not found on BP actor! %s "), SetsForTests::BpClassPath));
            return true;
        }

        //3. Assert        
        for (const UInputAction* i : Locomotion->AutoTestGetInputActions())
        {
            if (!i)
            {
                AddError(FString::Printf(TEXT("InputActionMove is not set in CustomLocomotionComponent! Index = %d ; %s "), counter, SetsForTests::BpClassPath));
                return true;
            }
            counter++;
        }
        counter = 0;

        ACustomPlayerController* PlayerControllerP = Cast<ACustomPlayerController>(TestActor->GetController());

        if (!PlayerControllerP)
        {
            AddError(FString::Printf(TEXT("PlayerController is not of type ACustomPlayerController! %s "), SetsForTests::BpClassPath));
            return true;
        }

        const auto& ImcArray = PlayerControllerP->AutoTestGetInputMappingContext();
        TestTrue(FString::Printf(TEXT("InputMappingContext is set in CustomLocomotionComponent. %s"), SetsForTests::BpClassPath), !ImcArray.IsEmpty());

        for (const FInputMappingContextWithPriority& i : PlayerControllerP->AutoTestGetInputMappingContext())
        {
            if (!i.MappingContext)
            {
                AddError(FString::Printf(TEXT("InputMappingContext has empty cell in PlayerControllerP! Index = %d; %s "), counter, SetsForTests::BpClassPath));
                return true;
            }
            counter++;
        }
        counter = 0;

        TestTrue(FString::Printf(TEXT("InputActionMove and InputMappingContext are set; %s "), SetsForTests::BpClassPath), true);
        return true;

    }));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS


/*
    #GameInstanceBase.h #GameInstanceBase.cpp
    #GeneralGameMode.h #GeneralGameMode.cpp
    #HubGameMode.h #HubGameMode.cpp
    #WorldGameMode.h #WorldGameMode.cpp

    #CustomPlayerController.h #CustomPlayerController.cpp
    #CustomLocomotionComponent.h #CustomLocomotionComponent.cpp
    #NetPlayerCharacter.h #NetPlayerCharacter.cpp 
        
    #InputTest.cpp #TestSettings.h
*/

/**
* @brief Open a map in the PIE - mod.
* @return true — if the map is opened.
*/
//IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOpenLevelTest, "ProjectA.Base.OpenLevel",
//	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
//
//bool FOpenLevelTest::RunTest(const FString & Parameters)
//{
//	//1. Arrange
//	const FString MapPath = SetsForTests::TestMapThirdPersonPath;
//
//	//2. Act
//	ADD_LATENT_AUTOMATION_COMMAND(FEditorLoadMap(MapPath));
//	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));
//
//	ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
//	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));
//
//	ADD_LATENT_AUTOMATION_COMMAND(FDelayedFunctionLatentCommand([this, MapPath]() -> bool
//		{
//			UWorld* World = GEditor->GetPIEWorldContext()->World();;
//			if (TestNotNull(TEXT("World must be valid after map load"), World))
//			{
//				return true;
//			}
//
//			const FString LoadedMap = World->GetMapName();
//			const TCHAR* TestDescription = *FString::Printf(TEXT("Loaded map name contains '%s'"), *MapPath);
//			const bool TestValue = LoadedMap.Contains(FPaths::GetBaseFilename(MapPath));
//
//			//3. Assert
//			TestTrue(TestDescription, TestValue);
//
//			return true;
//
//		})
//	);
//
//	return true;
//}


#endif // WITH_EDITOR