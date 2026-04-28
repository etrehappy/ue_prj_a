using UnrealBuildTool;

public class Quest: ModuleRules
{
    public Quest(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"
            , "Logger"
            , "Npc"
            , "Interaction"
            , "GameplayTags"
            , "NetCore"
            , "Inventory"
			, "Common"
		});
    }
}
