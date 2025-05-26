# Nanite Model Converter for VR (Unreal Engine 5.4.4)

A standalone, portable tool to convert high-poly FBX models (e.g., photogrammetry) into Nanite-enabled `.uasset` files that can be loaded at runtime in Unreal Engine VR projects. This tool is developed for use in a photogrammetry-based 3D model viewer for VR, requested by the Faculty of Earth Sciences at the University of Barcelona.

## 🌟 Purpose

Unreal Engine 5.4 does **not** support runtime generation of Nanite data. This tool provides a workaround to convert models **offline** into usable Nanite assets, allowing non-technical users (e.g., students and teachers) to prepare and use photogrammetric models in packaged VR projects.

## ✅ Features

* Convert `.fbx` models to Nanite-enabled `.uasset`
* Batch and automated processing via Unreal Editor commandlets
* Minimal Unreal Engine binaries required (portable setup in future)
* Runtime compatibility with `PakLoader` or `LoadObject()`
* Memory- and triangle-aware adaptive Nanite settings
* Optional metadata generation and JSON exports

## 🚀 Quick Start

### Clone the Repository

```bash
git clone https://github.com/<your-user>/NaniteModelConverter.git
cd NaniteModelConverter
```

### System Requirements

* Windows 10/11
* **Full official installation of Unreal Engine 5.4.4** is required for cooking
* Visual Studio 2019/2022 with C++ toolchain
* Assimp DLLs (already included)

> ⚠️ Currently, a full official installation is necessary due to cooking requirements.

### Build Instructions

1. Open `NaniteConverter.uproject` once in Unreal Editor to generate Visual Studio files.
2. Compile the solution in **Development Editor** mode from Visual Studio.
3. Final recomended structure:

```
/NaniteModelConverter
├── Engine_Binaries_Min/
├── Unreal_Conversion_Project/
├── Scripts/
├── Logs/
├── Output/
```

4. To build from command line:

```bash
"<PathToUE>\Engine\Build\BatchFiles\Build.bat" NaniteConverterEditor Win64 Development -project="<full_path>/NaniteConverter.uproject"
```

### Usage (Command Line)

```bash
NaniteConverter.bat "C:\Path\To\Model.fbx"
```

This will:

* Import the model
* Convert it to Nanite
* Attempt to cook the asset
* Output the cooked `.uasset` and associated files into `/Output/<ModelName>`

## 🔧 Developer Pipeline

* C++ Commandlet: `FBXToNaniteCommandlet`
* Plugin: `FBXToNanite`
* Texture and material automation via `TextureImporter`, `MaterialCreator`
* Maps and assets edited/duplicated via `MapEditor`

> 🧪 Originally prototyped in Python for fast testing, but due to limited Unreal integration, it was rewritten in C++ to access internal engine systems.

## ⚠️ Technical Limitations

* **StaticMesh cooking is partially implemented.** Cooked packages may not include the expected mesh data yet. Further debugging or manual cooking may be required.
* No runtime Nanite generation in UE5.4
* Full Unreal Engine install is required
* Windows-only
* High RAM usage for large assets (200M+ tris)
* Only static meshes supported (no skeletal mesh support)

> 💡 Attempting to load cooked `.pak` files showed that no StaticMesh actors were found, even though the conversion appeared successful. This issue is under investigation.

## ⚖️ License

MIT License

## 🤖 Authors

**Jose Adrián Guerra Viudez**
Developed in collaboration with the Faculty of Earth Sciences, University of Barcelona
Website: [jagv94.github.io](https://jagv94.github.io/index.html)

---
