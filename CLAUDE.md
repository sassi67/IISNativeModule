# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

A C++20 example/learning project demonstrating modern C++ features, built with CMake and vcpkg. Dependencies: GTest, fmt, spdlog.

## Build system

Dependencies are managed via **vcpkg**, bootstrapped automatically by the top-level `CMakeLists.txt`: if `VCPKG_ROOT` is unset, it clones vcpkg into `.vcpkg/`, bootstraps it, and installs `MODERNCPP_DEPENDENCIES` (GTest, fmt, spdlog) before `project()` is even declared. This means the *first* configure can take a while and requires network/git access; subsequent configures just auto-update the vcpkg checkout via `git pull`.

Building uses **CMake presets** (`CMakePresets.json`), targeting Visual Studio 2026 (`vs2026` generator) on Windows only — this project builds on Windows exclusively. Presets combine an architecture with a configuration:

- `windows-x86-32-debug` / `windows-x86-32-release`
- `windows-x86-64-debug` / `windows-x86-64-release`
- `windows-x86-64-clang` — MSVC arch/runtime but `ClangCL` toolset, debug only
- `windows-x86-64-coverage` — `ClangCL` toolset with `ENABLE_COVERAGE=ON` (adds `-fprofile-instr-generate -fcoverage-mapping`; fails fast via `message(FATAL_ERROR ...)` if the active compiler isn't Clang)
- `windows-x86-64-static-analysis` — see below

Each configure preset selects a toolchain file under `cmake/<preset>/toolchain.cmake`, which pins `CMAKE_SYSTEM_NAME`/`CMAKE_SYSTEM_PROCESSOR` and includes vcpkg's `vcpkg.cmake` toolchain. The top-level `CMakeLists.txt` infers the vcpkg triplet itself (`x64-windows` / `x86-windows`) from `CMAKE_GENERATOR_PLATFORM`.

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

Uses Ninja + clang-cl (not the VS generator, since multi-config VS generators can't emit `compile_commands.json`) with `/W4 -Wall -Wextra -Wshadow -Wconversion` and `CMAKE_EXPORT_COMPILE_COMMANDS=ON`. A `tidy` target runs `run-clang-tidy` against the generated compilation database, using checks from `.clang-tidy` (`bugprone-*`, `clang-analyzer-*`, `clang-diagnostic-*`, `modernize-*`, `performance-*`, `portability-*`). It falls back to the vendored copy in `scripts/run-clang-tidy.py` if `run-clang-tidy`/`run-clang-tidy.py` isn't found, and requires a Python 3 interpreter on `PATH` to run that vendored copy (CMake warns at configure time if one isn't found):

```powershell
cmake --preset windows-x86-64-static-analysis
cmake --build --preset windows-x86-64-static-analysis -t tidy
```

Fix suggestions land in `_build/windows-x86-64-static-analysis/tidy-fixes/` (one `.yaml` per file — merging into a single file needs PyYAML, which isn't a project dependency).

### Coverage (`windows-x86-64-coverage` preset)

`scripts/generate_coverage.bat` drives the full clang-cl + llvm-cov workflow end-to-end: configure → build → run tests with `LLVM_PROFILE_FILE` set → `llvm-profdata merge` → `llvm-cov show` (HTML report) → `llvm-cov report` (summary), then opens the HTML report. Run it from anywhere (it `cd`s to the repo root itself):

```powershell
scripts\generate_coverage.bat
```

Report lands at `_build\windows-x86-64-coverage\coverage-report\index.html`.

## Architecture

- **`src/`** — the main application (`CMakeLists.txt` defines target `ModernCppApp`, project `ModernCppApp`). Sources are glob-collected (`*.cpp`/`*.h` in the directory), so **new files under `src/` are picked up automatically at the next CMake configure** — no need to edit `src/CMakeLists.txt` when adding a file there.
  - `src/controller/`, `src/model/`, `src/view/` are currently empty placeholder directories (each holds only a `.gitignore`), suggesting an intended MVC split that hasn't been built out yet.
  - `src/utils/` is a separate static library target, `ModernCppUtils` (project `ModernCppUtils`), also glob-built and linked into both the app and the test binary. New utility files go here and are picked up the same way.
  - `src/module/` is the IIS native module itself: target `IISNativeModule`, a `SHARED` library (DLL) that exports `RegisterModule` via `iis_native_module.def`. `iis::HttpModule` (`http_module.h/.cpp`) overrides every overridable `CHttpModule` method with skeleton bodies returning `RQ_NOTIFICATION_CONTINUE`; `iis::HttpModuleFactory` creates instances; `register_module.cpp` currently subscribes to no notifications (`SetRequestNotifications(factory, 0, 0)`) — add `RQ_*` flags there as methods gain real implementations.
- **`test/`** — GTest-based tests, target/project `ModernCppTest`. Globs both `test/*.cpp` and `test/utils/*.cpp`, and links `ModernCppUtils`. `test/main.cpp` is a standard GTest `main()` running `RUN_ALL_TESTS()`. Mirror `src/utils/` layout under `test/utils/` for new utility tests (see `test/utils/smoke_test.cpp` as the existing example — one GTest `TEST` per behavior, wrapped in the same namespace as the code under test).
  - Test includes reference library headers as `<utils/smoke.h>` because `test/CMakeLists.txt` adds `test/../src` (i.e. `src/`) to the include path — so app/test code both include utils headers via the `utils/` prefix, not relative paths.
- Because both `src/` and `src/utils/` (and `test/`) use `file(GLOB ...)`, CMake must be **re-run** (re-configure, not just rebuild) after adding or removing source files for them to be picked up — GLOB results are cached at configure time.

## CI (`.github/workflows/ci.yml`)

- **CI does not use `cmake --preset`.** The repo's Windows presets pin an exact MSVC toolset patch version that isn't guaranteed to exist on GitHub-hosted runners, so CI instead invokes `cmake -G "Visual Studio 18 2026" -A x64 --toolchain cmake/windows-x86-64/toolchain.cmake` directly against `CMakeLists.txt`, with `-DCMAKE_C_COMPILER`/`-DCMAKE_CXX_COMPILER` selecting between an MSVC leg (`cl`) and a ClangCL leg (`clang-cl`). It passes `--toolchain <path>` (not `-DCMAKE_TOOLCHAIN_FILE=`) because `-D` was observed to truncate the value at its last `.`.
- If you change `cmake/windows-x86-64/toolchain.cmake` or the vcpkg bootstrap logic in the top-level `CMakeLists.txt`, check the inline comments in `ci.yml` first — several ordering constraints there (e.g. why `CMAKE_TOOLCHAIN_FILE` can't be pointed at vcpkg's own toolchain file directly) are load-bearing.

## Notes

- Minimum CMake version required is 4.2 (unusually new — verify your local CMake before assuming a configure failure is a code issue). CI installs `cmake >= 4.2` explicitly since GitHub-hosted runners don't have it preinstalled.
- The toolchain pins Visual Studio 2026 (`Visual Studio 18 2026`); local build presets additionally pin an exact MSVC toolset version (`14.44.35207` for build presets, `host=x64,version=14.50` for the static-analysis configure preset) — build failures on other VS/MSVC versions are expected without adjusting `CMakePresets.json`. CI sidesteps this (see above).
- `ModernCppUtils` (`src/utils/CMakeLists.txt`) is declared `add_library(... SHARED STATIC ...)` — despite the name, `STATIC` is the effective/last-wins keyword, so it builds as a static library.
