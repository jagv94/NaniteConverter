using UnrealBuildTool;
using System.Collections.Generic;

public class NaniteConverterTarget : TargetRules
{
    public NaniteConverterTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("NaniteConverter");
    }
}