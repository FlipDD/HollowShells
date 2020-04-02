// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Shells : ModuleRules
{
	public Shells(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "AIModule",
            "ApexDestruction",
            "Engine",
            "HeadMountedDisplay",
            "Http", 
            "InputCore",
            "Json", 
            "JsonUtilities", 
            "NavigationSystem",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Sockets",
            "Networking",
            "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore"
        });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
    }
}
