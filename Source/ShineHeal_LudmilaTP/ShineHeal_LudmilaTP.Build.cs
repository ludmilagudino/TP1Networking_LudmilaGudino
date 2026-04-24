// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShineHeal_LudmilaTP : ModuleRules
{
	public ShineHeal_LudmilaTP(ReadOnlyTargetRules Target) : base(Target)
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
			"ShineHeal_LudmilaTP",
			"ShineHeal_LudmilaTP/Variant_Platforming",
			"ShineHeal_LudmilaTP/Variant_Platforming/Animation",
			"ShineHeal_LudmilaTP/Variant_Combat",
			"ShineHeal_LudmilaTP/Variant_Combat/AI",
			"ShineHeal_LudmilaTP/Variant_Combat/Animation",
			"ShineHeal_LudmilaTP/Variant_Combat/Gameplay",
			"ShineHeal_LudmilaTP/Variant_Combat/Interfaces",
			"ShineHeal_LudmilaTP/Variant_Combat/UI",
			"ShineHeal_LudmilaTP/Variant_SideScrolling",
			"ShineHeal_LudmilaTP/Variant_SideScrolling/AI",
			"ShineHeal_LudmilaTP/Variant_SideScrolling/Gameplay",
			"ShineHeal_LudmilaTP/Variant_SideScrolling/Interfaces",
			"ShineHeal_LudmilaTP/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
