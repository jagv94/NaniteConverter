# NaniteConverter — FBX to Nanite converter for a VR model viewer (Unreal Engine 5.4)

> **Status: unfinished prototype, not maintained.** The FBX → Nanite import works; cooking and `.pak` packaging were left incomplete. Read [Known limitations](#known-limitations) before using it.

Tool built during my university internship (January–May 2025) at the Faculty of Earth Sciences, University of Barcelona, for a VR viewer of photogrammetry models (over 64 million triangles) aimed at non-technical users. **It was developed with substantial AI assistance** to meet the internship deadlines; the goals, decisions, integration and testing are mine.

## Why it exists

Unreal Engine 5.4 cannot generate Nanite data at runtime. NaniteConverter does the conversion **offline**: it imports an FBX into a small Unreal project, enables Nanite on the resulting Static Mesh and then tries to cook it and package it as a `.pak` that a packaged VR app could load later.

It is the second step of a pipeline whose first step is [FBXCloudConverter](https://github.com/jagv94/FBXCloudConverter) (point clouds / photogrammetry → FBX through CloudCompare).

## What it does

1. **`FBXToNanite` plugin (C++, editor module)**
   - Imports the FBX as a Static Mesh with Unreal's FBX importer (`UFbxFactory`), with auto-generated collision.
   - Looks for textures in the `<ModelName>.fbm/` folder next to the FBX (`*.jpg`), imports them and creates one material per texture (`TextureImporter`, `MaterialCreator`).
   - Enables Nanite on the mesh (`NaniteSettings.bEnabled = true`).
   - Duplicates `/Game/_StartupMap` into a temporary map that contains the mesh, so it can be cooked (`MapEditor`).
   - Handles the `_TRANSFORMED` suffix added by FBXCloudConverter.
2. **`FBXToNaniteCommandlet`**: runs that import headlessly: `-run=FBXToNaniteCommandlet -FBXPath="..."`.
3. **`NaniteConverter.bat`**: chains the whole process for **one FBX per run**: import → cook (`-run=cook` on the temporary map) → copy the cooked files → `UnrealPak` → clean-up.

A first prototype was written in Python and rewritten in C++ because the Python API did not expose the engine functions it needed.

## Requirements

- Windows 10/11.
- **Full official installation of Unreal Engine 5.4** (the cook and `UnrealPak` steps need it). The `.bat` expects it in `C:\Program Files\Epic Games\UE_5.4`; edit `ENGINE_ROOT` at the top of the file if yours is elsewhere.
- Visual Studio 2022 (or 2019) with the C++ game development workload, to build the editor module.
- Enough RAM for the model size: very large meshes (hundreds of millions of triangles) need a lot of memory during import.
- **Assimp binaries (not included in the repository).** `FBXToNaniteEditor.Build.cs` links `assimp-vc142-mt.lib` and expects `assimp-vc142-mt.dll` in `Plugins/FBXToNanite/Source/ThirdParty/assimp/lib/Win64/` and `bin/Win64/`, but `.lib` and `.dll` files are git-ignored. Either place an Assimp 5.x build for VS 2019 (vc142) there, or remove the `ASSIMP CONFIG` block from `Build.cs` (Assimp is not used by the import; see below).

## Build

1. Right-click `NaniteConverter.uproject` → *Generate Visual Studio project files*.
2. Build the `NaniteConverterEditor` target in **Development Editor | Win64**, or from the command line:

   ```bat
   "<UE_5.4>\Engine\Build\BatchFiles\Build.bat" NaniteConverterEditor Win64 Development -project="<full path>\NaniteConverter.uproject"
   ```

## Usage

### Full pipeline (`.bat`)

```bat
NaniteConverter.bat "C:\Models\MyModel.fbx"
```

- Spaces in the file name are replaced with `_`.
- Logs: `Log\<ModelName>\Import.log`, `Cook.log`, `Pak.log`, `Error.log`.
- Expected output: `Output\<ModelName>\<ModelName>.pak`.
- **The script deletes `Saved\`, the imported assets in `Content\FBX_Imported\<ModelName>\` and the temporary map when it finishes.** Copy anything you want to inspect before it runs, or comment out the clean-up block.

### Import only (commandlet)

```bat
"<UE_5.4>\Engine\Binaries\Win64\UnrealEditor.exe" "<full path>\NaniteConverter.uproject" -run=FBXToNaniteCommandlet -FBXPath="C:\Models\MyModel.fbx" -log
```

This leaves the Nanite Static Mesh, its textures and materials in `Content/FBX_Imported/<ModelName>/`, and the temporary map in `Content/`. You can open them in the editor.

## Known limitations

- **Cooking is only partially implemented.** The cooked packages did not contain the mesh: loading the generated `.pak` found no Static Mesh.
- **The full pipeline (photogrammetry → `.pak` loaded in VR) never worked end to end.** The runtime loader (`PakLoader`, in the viewer project) was never tested with a valid `.pak`.
- One model per run: there is no batch mode over a folder (you can call the `.bat` from your own loop).
- `Config/DefaultNaniteConverter.ini` describes adaptive settings (triangle thresholds, memory, profiles) that **the code does not read yet**. They are notes for future work, not active features.
- The Assimp library is linked in `FBXToNaniteEditor.Build.cs` but the import uses Unreal's FBX importer. Assimp is left over from an earlier attempt at runtime import.
- Only static meshes; Windows only; the engine path is hard-coded in the `.bat`.

## Third-party code

- `Plugins/FBXToNanite/Source/ThirdParty/assimp/include/` contains the headers of the [Open Asset Import Library (Assimp)](https://github.com/assimp/assimp), version 5.x (they match the 5.2.5–5.3.x releases), under the **BSD-3-Clause** license — see [`Plugins/FBXToNanite/Source/ThirdParty/assimp/LICENSE`](Plugins/FBXToNanite/Source/ThirdParty/assimp/LICENSE).

## License

MIT — see [LICENSE](LICENSE). The MIT license covers my code; third-party code keeps its own license.

## Author

Jose Adrián Guerra Viudez — [jagv94.github.io](https://jagv94.github.io)
Internship project for the Faculty of Earth Sciences, University of Barcelona (2025).
