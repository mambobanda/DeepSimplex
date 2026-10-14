# DeepSimplex

A native C++ mesh and geometry-processing library with a small example executable. Buildable from the included Visual Studio solution; provides mesh data structures, mesh operators, point-cloud utilities and a bundled libtess tessellator.

## Stack
- **Language(s):** C++ (primary)
- **Framework / runtime:** Visual Studio / MSBuild (solution: `DeepSimplex.sln`)
- **Notable libraries:** bundled `libtess` (tessellation), C++ Standard Library

## How it's organized
```
DeepSimplex.sln                     # Visual Studio solution (Debug/Release x64)
DeepSimplex/                        # project source
  Deep Simplex.smproj               # project metadata
  DeepSimplex.vcxproj               # Visual C++ project
  main.cpp                          # program entry point for demo/executable
  Ds*.cpp / Ds*.h                   # mesh implementation and operators
  libtess/                          # vendored tessellation library
.gitattributes
.gitignore
```

**How it fits together:** The solution builds the DeepSimplex project which compiles a set of `Ds*` modules (mesh, mesh operators, point-cloud utilities) and links a small demo executable defined in `main.cpp`. `libtess` is included for tessellation routines used by mesh builders.

## How to build & run (Windows / Visual Studio)
1. Clone the repository:

```bash
git clone https://github.com/mambobanda/DeepSimplex.git
cd DeepSimplex
```

2. Build in Visual Studio
- Open `DeepSimplex.sln` in Visual Studio (recommended: VS 2017/2019/2022). Select `Release | x64` and build the solution.

3. Or build from the command line with MSBuild:

```bash
msbuild DeepSimplex.sln /p:Configuration=Release /p:Platform=x64
```

4. Run the demo executable
- After building, find the produced executable in the project's output folder (check the project properties / Output Path). Example possible location:

```
DeepSimplex\x64\Release\DeepSimplex.exe
```

If the executable is not in that exact path, open the project's `OutputPath` in the `.vcxproj` to find the precise location.

## Key files
- Solution: https://github.com/mambobanda/DeepSimplex/blob/master/DeepSimplex.sln
- Project: https://github.com/mambobanda/DeepSimplex/blob/master/DeepSimplex/DeepSimplex.vcxproj
- Entry point: https://github.com/mambobanda/DeepSimplex/blob/master/DeepSimplex/main.cpp
- Mesh sources (examples): https://github.com/mambobanda/DeepSimplex/tree/master/DeepSimplex
- Vendored tessellator: https://github.com/mambobanda/DeepSimplex/tree/master/DeepSimplex/libtess

## Notes & next steps
- This repo is Visual Studio–centric. If you’d like cross-platform builds, I can add a CMakeLists.txt and CI to build on Linux/macOS.
- Consider adding a short example input file or an examples/ directory so users can run the demo without crafting inputs.

---

*README generated automatically by a repository scan. Please review and adjust build instructions, required SDK/toolset versions, and any missing dependency notes.*
