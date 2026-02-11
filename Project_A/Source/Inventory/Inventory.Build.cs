using UnrealBuildTool;

public class Inventory: ModuleRules
{
    public Inventory(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"
			, "GameplayTags"
            , "WeaponSystemPlugin"
            , "Logger"
		    , "NetCore" });
    }
}
