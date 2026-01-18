using UnrealBuildTool;

public class Common: ModuleRules
{
    public Common(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"
            , "Logger"
			, "WeaponSystemPlugin"
		});
    }
}
