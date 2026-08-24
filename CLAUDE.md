# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

A native HTTP module for IIS to experiment with new ways of unit testing. C++20, built with CMake and vcpkg. Dependencies: GTest, fmt, spdlog.

## Build system

Dependencies are managed via **vcpkg**, bootstrapped automatically by the top-level `CMakeLists.txt`: if `VCPKG_ROOT` is unset, it clones vcpkg into `.vcpkg/` and bootstraps it before `project()` is even declared; the dependencies declared in `vcpkg.json` (GTest, fmt, spdlog) are then installed by vcpkg's manifest-mode resolution during configure. This means the *first* configure can take a while and requires network/git access; subsequent configures just auto-update the vcpkg checkout via `git pull`.

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

Run a single test binary directly with GTest filters (there is currently one test target, `IISNativeModuleTest`, registered with CTest as a single opaque test — use the binary's own filter flag to select individual cases):

```powershell
_build\windows-x86-64-debug\test\Debug\IISNativeModuleTest.exe --gtest_filter=TestCore.TestModuleHandlerOnBeginRequestForbiddenUrl
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

- **`src/`** — holds no sources of its own; `src/CMakeLists.txt` only aggregates the two real targets below. There is no application executable: the deliverable is the DLL that IIS loads.
  - `src/core/` is the IIS/Windows-independent layer: static library target `IISNativeModuleCore` (namespace `core`), exporting its headers as `<core/...>` via a PUBLIC include dir. It holds `Verdict` (the platform-free mirror of `REQUEST_NOTIFICATION_STATUS`), the interfaces `IRequest`/`IResponse`/`IContext`/`IEventProvider`/`IModule` mirroring their IIS counterparts, and `ModuleHandler` — the actual request logic (403 + reason `Forbidden by IISRequestLevelModule` for cooked URL paths ending in `/forbidden`, `X-IISRequestLevelModule: 1` header otherwise). Nothing in this directory may include Windows or IIS headers; HRESULT-shaped values travel as `std::int32_t`.
  - `src/module/` is the IIS native module itself: target `IISNativeModule`, a `SHARED` library (DLL) that exports `RegisterModule` via `iis_native_module.def` and links `IISNativeModuleCore`. Files here — as in `src/core` and `test/core` — are named after the class they hold (`IISRequestLevelModule.h/.cpp` etc., PascalCase). `iis::IISRequestLevelModule` is a thin adapter: its constructor takes a non-owning `core::IModule*`, and `OnBeginRequest` wraps the live IIS objects in the `IISContext`/`IISRequest`/`IISResponse`/`IISEventProvider` adapters (one file pair per class), delegates to the injected module, and maps the returned `Verdict` back to a `REQUEST_NOTIFICATION_STATUS`. `iis::IISRequestLevelModuleFactory` owns the shared (stateless) `core::ModuleHandler` and injects it into every instance it creates; `register_module.cpp` subscribes to `RQ_BEGIN_REQUEST` only — add further `RQ_*` flags there as methods gain implementations.
- **`test/`** — GTest-based tests, target/project `IISNativeModuleTest`. Globs `test/*.cpp` and `test/core/*.cpp`, and links `IISNativeModuleCore`. `test/main.cpp` is a standard GTest `main()` running `RUN_ALL_TESTS()`. Mirror the `src/` layout under `test/` for new tests (one GTest `TEST` per behavior, wrapped in the same namespace as the code under test). `test/core/ModuleHandlerTest.cpp` exercises `core::ModuleHandler` through hand-rolled mocks of the core interfaces — the pattern to follow for testing module logic without IIS.
  - Tests include core headers as `<core/...>`, which resolves because `IISNativeModuleCore` exports `src/` as a PUBLIC include directory — not through any include path set on the test target itself.
- Because every target uses `file(GLOB ...)`, CMake must be **re-run** (re-configure, not just rebuild) after adding or removing source files for them to be picked up — GLOB results are cached at configure time.

## CI (`.github/workflows/ci.yml`)

- **CI does not use `cmake --preset`.** The repo's Windows presets pin an exact MSVC toolset patch version that isn't guaranteed to exist on GitHub-hosted runners, so CI instead invokes `cmake -G "Visual Studio 18 2026" -A x64 --toolchain cmake/windows-x86-64/toolchain.cmake` directly against `CMakeLists.txt`, with `-DCMAKE_C_COMPILER`/`-DCMAKE_CXX_COMPILER` selecting between an MSVC leg (`cl`) and a ClangCL leg (`clang-cl`). It passes `--toolchain <path>` (not `-DCMAKE_TOOLCHAIN_FILE=`) because `-D` was observed to truncate the value at its last `.`.
- If you change `cmake/windows-x86-64/toolchain.cmake` or the vcpkg bootstrap logic in the top-level `CMakeLists.txt`, check the inline comments in `ci.yml` first — several ordering constraints there (e.g. why `CMAKE_TOOLCHAIN_FILE` can't be pointed at vcpkg's own toolchain file directly) are load-bearing.

## Notes

- Minimum CMake version required is 4.2 (unusually new — verify your local CMake before assuming a configure failure is a code issue). CI installs `cmake >= 4.2` explicitly since GitHub-hosted runners don't have it preinstalled.
- The toolchain pins Visual Studio 2026 (`Visual Studio 18 2026`); local build presets additionally pin an exact MSVC toolset version (`14.44.35207` for build presets, `host=x64,version=14.50` for the static-analysis configure preset) — build failures on other VS/MSVC versions are expected without adjusting `CMakePresets.json`. CI sidesteps this (see above).
- `vcpkg.json` still declares `fmt` and `spdlog` (and the top-level `CMakeLists.txt` still `find_package`s them), but no target links them any more — they are kept available for future logging/formatting work.
