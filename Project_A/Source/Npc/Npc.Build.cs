using UnrealBuildTool;

public class Npc: ModuleRules
{
    public Npc(ReadOnlyTargetRules Target) : base(Target)
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
			"NetCore",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"NavigationSystem",
			"GameplayTasks",
			"Common",
			"Logger",
			"WeaponSystemPlugin",
			"Inventory",
			"Dialogue"
		});
	}
}
