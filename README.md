Nanite Model Converter for VR (Unreal Engine 5.4.4)

A standalone, portable tool to convert high-poly FBX models (e.g., photogrammetry) into Nanite-enabled .uasset files that can be loaded at runtime in Unreal Engine VR projects. This tool is developed for use in a photogrammetry-based 3D model viewer for VR, requested by the Faculty of Earth Sciences at the University of Barcelona.

🌟 Purpose

Unreal Engine 5.4 does not support runtime generation of Nanite data. This tool provides a workaround to convert models offline into usable Nanite assets, allowing non-technical users (e.g., students and teachers) to prepare and use photogrammetric models in packaged VR projects.

✅ Features

Convert .fbx models to Nanite-enabled .uasset

Batch and automated processing via Unreal Editor commandlets

Minimal Unreal Engine binaries required (portable setup)

Runtime compatibility with PakLoader or LoadObject()

Memory- and triangle-aware adaptive Nanite settings

Optional metadata generation and JSON exports

🚀 Quick Start

Clone the Repository

git clone https://github.com/<your-user>/NaniteModelConverter.git
cd NaniteModelConverter

Prerequisites

Windows 10/11

Unreal Engine 5.4.4 (or just the UnrealEditor.exe + required modules)

Visual Studio 2019/2022 with C++ toolchain

Assimp DLLs (already included)

Build Instructions

Open NaniteConverter.uproject once in Unreal Editor to generate Visual Studio files.

Compile the solution in Development Editor mode from Visual Studio.

Ensure the folder structure remains:

/NaniteModelConverter
├── Engine_Binaries_Min/
├── Proyecto_Unreal_Conversion/
├── Scripts/
├── UI/
├── Models_Convertidos/

To build from command line:

"<PathToUE>\Engine\Build\BatchFiles\Build.bat" NaniteConverterEditor Win64 Development -project="<full_path>/NaniteConverter.uproject"

Usage (Command Line)

NaniteConverter.bat "C:\Path\To\Model.fbx"

This will:

Import the model

Convert it to Nanite

Cook the asset

Output the cooked .uasset and associated files into /Output/<ModelName>

🔧 Developer Pipeline

C++ Commandlet: FBXToNaniteCommandlet

Plugin: FBXToNanite

Texture and material automation via TextureImporter, MaterialCreator

Maps and assets edited/duplicated via MapEditor

⚠️ Technical Limitations

No runtime Nanite generation in UE5.4

Windows-only

High RAM usage for large assets (200M+ tris)

Only static meshes supported (no skeletal mesh support)

⚖️ License

MIT License

🤖 Authors

Jose Adrián Guerra ViudezDeveloped in collaboration with the Faculty of Earth Sciences, University of BarcelonaWebsite: jagv94.github.io
