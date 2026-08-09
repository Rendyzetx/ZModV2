# ZModV2

ZModV2 is a modern, feature-rich DirectX 11 / IL2CPP internal modification framework for Pixel Worlds. Built with an architecture utilizing MinHook, Kiero, and a customized Dear ImGui interface with modern styling.

---

## Features

- **Rendering & UI**: High-performance DirectX 11 overlay powered by Dear ImGui with customizable themes, animations, smooth easing, and font rendering.
- **IL2CPP Method Resolution**: Name-keyed runtime resolver that adapts to Unity IL2CPP builds.
- **Automation Engines**:
  - AutoMine (dynamic pathfinding, gemstone & ore detection)
  - AutoNether (dungeon pathing & item collection)
  - AutoFish & AutoCollect
- **Pathfinding**: 8-directional tile grid A* algorithm with airborne constraints, obstacle filtering, and movement smoothing.
- **Visuals & ESP**: Rich entity, tile, boss, and item world-to-screen overlay rendering.
- **Network Layer**: BSON packet interceptor and builder framework.

---

## Building from Source

### Prerequisites
- Visual Studio 2022 (Community or higher)
- C++ Desktop Development Workload (MSVC v143 toolset)
- Windows SDK 10.0 (or newer)

### Build Steps
1. Clone the repository:
   ```sh
   git clone https://github.com/Changedit/ZModV2.git
   cd ZModV2
   ```
2. Build via the batch script or Visual Studio:
   - Run `_build.bat` in the root folder, or
   - Open `CodeSpark.sln` in Visual Studio and build under `Release | x64`.

---

## License

This project is released for educational and research purposes.
