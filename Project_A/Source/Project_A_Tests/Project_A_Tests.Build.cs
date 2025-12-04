using UnrealBuildTool;

public class Project_A_Tests: ModuleRules
{
    public Project_A_Tests(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "Project_A", "UnrealEd"});        
    }
}
