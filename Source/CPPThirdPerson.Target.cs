// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CPPThirdPersonTarget : TargetRules
{
	public CPPThirdPersonTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Game;
		ExtraModuleNames.Add("CPPThirdPerson");
		
		// bUseLoggingInShipping = true;
		// bOverrideBuildEnvironment = true;
		// BuildEnvironment = TargetBuildEnvironment.Unique;
	}
}
