// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MazeTester : ModuleRules
{
	public MazeTester(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "MazeGenerator" });
        PrivateIncludePaths.AddRange(new string[] { "MazeGenerator/Private" });
        PublicIncludePaths.AddRange(new string[] { "MazeGenerator/Public" });
    }

}
