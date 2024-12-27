// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CPPThirdPersonEditorTarget : TargetRules
{
	public CPPThirdPersonEditorTarget(TargetInfo Target) : base(Target)
	{
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		Type = TargetType.Editor;
		ExtraModuleNames.Add("CPPThirdPerson");
	}
}
