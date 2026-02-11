using UnrealBuildTool;

public class Interaction: ModuleRules
{
    public Interaction(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"            
			, "Logger"});
    }
}
