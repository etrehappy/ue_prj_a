using UnrealBuildTool;

public class Enemy: ModuleRules
{
    public Enemy(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core"
            , "CoreUObject"
            , "Engine"
            , "GameplayTags"
            , "Common"
            , "Logger"
            , "Npc"
			, "WeaponSystemPlugin"
            , "Interaction"
		});
    }
}
