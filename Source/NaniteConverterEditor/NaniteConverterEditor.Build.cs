using UnrealBuildTool;

public class NaniteConverterEditor : ModuleRules
{
    public NaniteConverterEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "AssetTools",
                "EditorScriptingUtilities",
                "NaniteBuilder",
                "RenderCore",
                "RHI"
            }
        );

        PrivateDependencyModuleNames.AddRange(new string[] {
            "FBXToNaniteEditor"
        });
    }
}