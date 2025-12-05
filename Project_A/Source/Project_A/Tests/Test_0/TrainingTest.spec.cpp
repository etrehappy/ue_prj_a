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

#endif // WITH_DEV_AUTOMATION_TESTS
