using UnrealBuildTool;

public class Dialogue: ModuleRules
{
    public Dialogue(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"
		, "Logger"
        , "Quest"
		, "GameplayTags"
        , "Interaction"
		});
    }
}
