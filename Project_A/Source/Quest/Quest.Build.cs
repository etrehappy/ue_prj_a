using UnrealBuildTool;

public class Quest: ModuleRules
{
    public Quest(ReadOnlyTargetRules Target) : base(Target)
    {		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"Interaction"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Logger",
			"NetCore",
			"Inventory",
			"Common"
		});
	}
}
