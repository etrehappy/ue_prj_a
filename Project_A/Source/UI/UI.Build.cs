using UnrealBuildTool;

public class UI: ModuleRules
{
    public UI(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"

			,"UMG"
            , "Logger"
            , "Inventory"
            , "WeaponSystemPlugin"
		    , "NetCore"
			, "UMG"
            , "Slate"
            , "SlateCore"
            , "InputCore"
            , "Common"
		});
    }
}
