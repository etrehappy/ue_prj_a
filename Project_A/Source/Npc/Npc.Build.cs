using UnrealBuildTool;

public class Npc: ModuleRules
{
    public Npc(ReadOnlyTargetRules Target) : base(Target)
    {
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"NetCore",
			"AIModule",			
			"StateTreeModule",
			"GameplayStateTreeModule",
			"NavigationSystem",
			"GameplayTasks",
			"Common",
			"Logger",
			"WeaponSystemPlugin"
		});
	}
}
