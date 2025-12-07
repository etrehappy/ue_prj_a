// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Project_AServerTarget : TargetRules
{
	public Project_AServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
        bUseLoggingInShipping = true;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        ExtraModuleNames.Add("Project_A");
	}
}
