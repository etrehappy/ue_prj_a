using UnrealBuildTool;

public class Common: ModuleRules
{
    public Common(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicDependencyModuleNames.AddRange(new string[]
			 {
				"Logger"
			 }
		);

		PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"
            //, "Logger"
			, "WeaponSystemPlugin"
			, "GameplayTags"
		});
    }
}
