// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Project_ATarget : TargetRules
{
	public Project_ATarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        //bUseLoggingInShipping = true;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        ExtraModuleNames.Add("Project_A");
	}
}
