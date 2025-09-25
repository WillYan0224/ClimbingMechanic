// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ClimbingMechanic : ModuleRules
{
	public ClimbingMechanic(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ClimbingMechanic",
			"ClimbingMechanic/Variant_Platforming",
			"ClimbingMechanic/Variant_Combat",
			"ClimbingMechanic/Variant_Combat/AI",
			"ClimbingMechanic/Variant_SideScrolling",
			"ClimbingMechanic/Variant_SideScrolling/Gameplay",
			"ClimbingMechanic/Variant_SideScrolling/AI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
