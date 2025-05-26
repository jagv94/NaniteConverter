using UnrealBuildTool;
using System.Collections.Generic;

public class NaniteConverterEditorTarget : TargetRules
{
    public NaniteConverterEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        bBuildDeveloperTools = true;
        bBuildTargetDeveloperTools = true;
        bUseLoggingInShipping = false;
        bCompileAgainstEngine = true;
        bCompileAgainstCoreUObject = true;

        ExtraModuleNames.AddRange(new[] {
            "NaniteConverterEditor"
        });
    }
}