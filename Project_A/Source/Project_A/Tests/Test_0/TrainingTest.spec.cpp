#include "Async/Async.h"
#include "Containers/Ticker.h"  
#include "Editor.h" 
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet2/DebuggerCommands.h"
#include "Tests/AutomationCommon.h"
#include "TestSettings.h"


#if WITH_DEV_AUTOMATION_TESTS


BEGIN_DEFINE_SPEC(FShowWorldTypeSpec, "TrainingTests.Spec.Show WorldType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
	UWorld* World{};
END_DEFINE_SPEC(FShowWorldTypeSpec)
void FShowWorldTypeSpec::Define()
{
	//@todo write my expectations here

	Describe("LogWorldType()", [this]()
	{
		BeforeEach([this]()
		{
			World = GEditor->GetEditorWorldContext().World();
			AddForTests::LogWorldType(World);
		});


		It("should display the type of the world(-s) when the world is not equal to null", [this]()
			{				
				TestNotNull(TEXT("World was found"), World);
			});
	});
}



BEGIN_DEFINE_SPEC(FOpenMapPIESpec, "TrainingTests.Spec.OpenMapPIE", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
UWorld* World = nullptr;
END_DEFINE_SPEC(FOpenMapPIESpec)

void FOpenMapPIESpec::Define()
{    
    LatentBeforeEach(EAsyncExecution::ThreadPool, FTimespan::FromSeconds(10.0), [this](const FDoneDelegate& Done)
    {       
        AsyncTask(ENamedThreads::GameThread, [this, Done]() mutable
        {
            const FString MapToTest = SetsForTests::TestMapThirdPersonPath; 
            if (!AutomationOpenMap(MapToTest))
            {                
                Done.Execute();
                return;
            }
                        
            if (FPlayWorldCommands::GlobalPlayWorldActions.IsValid())
            {
                FPlayWorldCommands::GlobalPlayWorldActions->ExecuteAction(FPlayWorldCommands::Get().PlayInViewport.ToSharedRef());
            }
            
            FTickerDelegate TickDelegate = FTickerDelegate::CreateLambda([this, Done](float DeltaSeconds) -> bool
            {
                if (GEngine == nullptr)
                {
                    return true; 
                }

                for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
                {                   
                    if (Ctx.World() && Ctx.WorldType == EWorldType::PIE)
                    {
                        World = Ctx.World();
                        Done.Execute();
                        return false; 
                    }
                }

                return true; 
            });

            FTSTicker::GetCoreTicker().AddTicker(TickDelegate, 0.1f);
        });
    });


It("should open the map and run in PIE (UWorld must be PIE)", [this]()
    {
        TestNotNull("World must be valid after opening and starting PIE", World);
        if (World)
        {
            TestEqual("World type should be PIE", World->WorldType, EWorldType::PIE);
        }
    });

}

#endif // WITH_DEV_AUTOMATION_TESTS
