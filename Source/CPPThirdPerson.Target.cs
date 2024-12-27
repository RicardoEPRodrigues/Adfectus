// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CPPThirdPersonTarget : TargetRules
{
	public CPPThirdPersonTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("CPPThirdPerson");
		
		// bUseLoggingInShipping = true;
		// bOverrideBuildEnvironment = true;
		// BuildEnvironment = TargetBuildEnvironment.Unique;
	}
}
