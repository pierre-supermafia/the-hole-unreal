// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheHole : ModuleRules
{
	public TheHole(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OSC" });

		PrivateDependencyModuleNames.AddRange(new string[] { "OSC" });

		// PrivateIncludePathModuleNames.AddRange(new string[] { "OSC/Public", "OSC/Private" });

		PublicIncludePaths.AddRange(new string[] { "OSC/Public", "OSC/Classes" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
