# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

A C++20 example/learning project demonstrating modern C++ features, built with CMake and vcpkg. Dependencies: GTest, fmt, spdlog.

## Build system

Dependencies are managed via **vcpkg**, bootstrapped automatically by the top-level `CMakeLists.txt`: if `VCPKG_ROOT` is unset, it clones vcpkg into `.vcpkg/`, bootstraps it, and installs `MODERNCPP_DEPENDENCIES` (GTest, fmt, spdlog) before `project()` is even declared. This means the *first* configure can take a while and requires network/git access; subsequent configures just auto-update the vcpkg checkout via `git pull`.

Building uses **CMake presets** (`CMakePresets.json`), targeting Visual Studio 2026 (`vs2026` generator) on Windows only. Presets combine an architecture (`windows-x86-32` / `windows-x86-64`) with a configuration (`debug` / `release`):

- `windows-x86-32-debug`
- `windows-x86-32-release`
- `windows-x86-64-debug`
- `windows-x86-64-release`

Each configure preset selects a toolchain file under `cmake/<preset>/toolchain.cmake`, which pins `CMAKE_SYSTEM_NAME`/`CMAKE_SYSTEM_PROCESSOR` and includes vcpkg's `vcpkg.cmake` toolchain.

### Common commands (PowerShell)

Configure, build, and test using a preset (x86-64 debug shown; swap in any preset name above):

```powershell
cmake --preset windows-x86-64-debug
cmake --build --preset windows-x86-64-debug
ctest --preset windows-x86-64-debug
```

Build artifacts land in `_build/<presetName>/`.

Run a single test binary directly with GTest filters (there is currently one test target, `ModernCppTest`, registered with CTest as a single opaque test — use the binary's own filter flag to select individual cases):

```powershell
_build\windows-x86-64-debug\test\Debug\ModernCppTest.exe --gtest_filter=TestUtils.TestSmokeGetHello
```

Run the app:

```powershell
_build\windows-x86-64-debug\src\Debug\ModernCppApp.exe
```

### Static analysis (`windows-x86-64-static-analysis` preset)

Uses Ninja + clang-cl (not the VS generator, since multi-config VS generators can't emit `compile_commands.json`) with `/W4 -Wall -Wextra -Wshadow -Wconversion` and `CMAKE_EXPORT_COMPILE_COMMANDS=ON`. A `tidy` target runs `run-clang-tidy` against the generated compilation database (falls back to the vendored copy in `scripts/run-clang-tidy.py` if it isn't on `PATH`):

```powershell
cmake --preset windows-x86-64-static-analysis
cmake --build --preset windows-x86-64-static-analysis -t tidy
```

Fix suggestions land in `_build/windows-x86-64-static-analysis/tidy-fixes/` (one `.yaml` per file — merging into a single file needs PyYAML, which isn't a project dependency).

## Architecture

- **`src/`** — the main application (`CMakeLists.txt` defines target `ModernCppApp`, project `ModernCppApp`). Sources are glob-collected (`*.cpp`/`*.h` in the directory), so **new files under `src/` are picked up automatically at the next CMake configure** — no need to edit `src/CMakeLists.txt` when adding a file there.
  - `src/controller/`, `src/model/`, `src/view/` are currently empty placeholder directories (each holds only a `.gitignore`), suggesting an intended MVC split that hasn't been built out yet.
  - `src/utils/` is a separate static library target, `ModernCppUtils` (project `ModernCppUtils`), also glob-built and linked into both the app and the test binary. New utility files go here and are picked up the same way.
- **`test/`** — GTest-based tests, target/project `ModernCppTest`. Globs both `test/*.cpp` and `test/utils/*.cpp`, and links `ModernCppUtils`. `test/main.cpp` is a standard GTest `main()` running `RUN_ALL_TESTS()`. Mirror `src/utils/` layout under `test/utils/` for new utility tests (see `test/utils/smoke_test.cpp` as the existing example — one GTest `TEST` per behavior, wrapped in the same namespace as the code under test).
  - Test includes reference library headers as `<utils/smoke.h>` because `test/CMakeLists.txt` adds `test/../src` (i.e. `src/`) to the include path — so app/test code both include utils headers via the `utils/` prefix, not relative paths.
- Because both `src/` and `src/utils/` (and `test/`) use `file(GLOB ...)`, CMake must be **re-run** (re-configure, not just rebuild) after adding or removing source files for them to be picked up — GLOB results are cached at configure time.

## Notes

- Minimum CMake version required is 4.2 (unusually new — verify your local CMake before assuming a configure failure is a code issue).
- The toolchain pins Visual Studio 2026 (`Visual Studio 18 2026`) with a specific MSVC toolset version (`14.44.35207`/`14.50`) — build failures on other VS/MSVC versions are expected without adjusting `CMakePresets.json`.
