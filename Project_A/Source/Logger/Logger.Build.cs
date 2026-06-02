using UnrealBuildTool;

public class Logger: ModuleRules
{
    public Logger(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});      

    }
}
