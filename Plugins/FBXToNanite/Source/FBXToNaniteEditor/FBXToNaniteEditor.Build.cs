using System.IO;
using UnrealBuildTool;

public class FBXToNaniteEditor : ModuleRules
{
    public FBXToNaniteEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "RenderCore",
            "RHI",
            "ImageWrapper",
            "InputCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "RawMesh",                          
            "UnrealEd",
            "MainFrame",
            "Projects",
            "SlateCore",
            "ToolMenus",
            "InputCore",
            "AssetTools",
            "LevelEditor",
            "AssetRegistry",
            "ContentBrowser",
            "DesktopPlatform",
            "PropertyEditor",
            "MaterialEditor",
            "EditorScriptingUtilities"
        });

        PrivateIncludePathModuleNames.AddRange(new string[]
        {
            "Core",
            "Engine",
            "UnrealEd",
            "EditorStyle",
            "EditorFramework",
            "EditorSubsystem"
        });

        DynamicallyLoadedModuleNames.AddRange(new string[] { });

        // ==== ASSIMP CONFIG ====
        string AssimpBasePath = Path.Combine(ModuleDirectory, "..", "ThirdParty", "assimp");
        string AssimpIncludePath = Path.Combine(AssimpBasePath, "include");
        string AssimpLibPath = Path.Combine(AssimpBasePath, "lib/Win64");
        string AssimpBinPath = Path.Combine(AssimpBasePath, "bin/Win64");

        PublicIncludePaths.Add(AssimpIncludePath);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string AssimpLibFile = Path.Combine(AssimpLibPath, "assimp-vc142-mt.lib");
            string AssimpDllFile = "assimp-vc142-mt.dll";

            PublicSystemLibraryPaths.Add(AssimpLibPath);
            PublicAdditionalLibraries.Add(AssimpLibFile);

            PublicDelayLoadDLLs.Add(AssimpDllFile);

            RuntimeDependencies.Add(Path.Combine("$(PluginDir)", "Source/ThirdParty/assimp/bin/Win64", AssimpDllFile));
        }
    }
}