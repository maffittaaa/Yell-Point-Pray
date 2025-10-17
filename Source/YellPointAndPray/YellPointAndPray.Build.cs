// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class YellPointAndPray : ModuleRules
{
	public YellPointAndPray(ReadOnlyTargetRules Target) : base(Target)
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
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"YellPointAndPray",
			"YellPointAndPray/Variant_Horror",
			"YellPointAndPray/Variant_Horror/UI",
			"YellPointAndPray/Variant_Shooter",
			"YellPointAndPray/Variant_Shooter/AI",
			"YellPointAndPray/Variant_Shooter/UI",
			"YellPointAndPray/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
