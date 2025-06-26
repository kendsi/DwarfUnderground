// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DwarfUnderground : ModuleRules
{
	public DwarfUnderground(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput",
			"UMG", "Slate", "SlateCore", "GameplayTags"
		});
	}
}