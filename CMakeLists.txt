cmake_minimum_required(VERSION 4.2)

# vcpkg checkout pin: the same commit as the builtin-baseline in
# vcpkg.json, so the tool, its scripts, and the port baseline all come
# from one audited revision (supply-chain: never execute unpinned,
# auto-updated code at configure time). To bump vcpkg, update BOTH the
# baseline in vcpkg.json and this pin to the new commit, then wipe
# .vcpkg/ (or let the checkout below fetch it).
set(VCPKG_PINNED_COMMIT "d92484ed3c5020c6679d095ad3e5add907887b62")

# If the environment variable VCPKG_ROOT is set, that checkout is
# user-managed: no git operations are performed on it (only the
# bootstrap-if-missing below). Otherwise a local .vcpkg clone is
# created and pinned to VCPKG_PINNED_COMMIT.
if(NOT DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.vcpkg)
    set(VCPKG_CHECKOUT_IS_MANAGED TRUE)
else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
    set(VCPKG_CHECKOUT_IS_MANAGED FALSE)
endif()

if(NOT EXISTS ${VCPKG_ROOT})
    if(NOT VCPKG_CHECKOUT_IS_MANAGED)
        message(FATAL_ERROR "***** FATAL ERROR: VCPKG_ROOT points at a non-existing path: ${VCPKG_ROOT} *****")
    endif()
    message("Cloning vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_ROOT}/README.md)
    message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
endif()

if(VCPKG_CHECKOUT_IS_MANAGED)
    execute_process(
        COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${VCPKG_ROOT}
        OUTPUT_VARIABLE VCPKG_CURRENT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT VCPKG_CURRENT_COMMIT STREQUAL VCPKG_PINNED_COMMIT)
        message("Pinning vcpkg to ${VCPKG_PINNED_COMMIT}")
        execute_process(
            COMMAND git -c advice.detachedHead=false checkout ${VCPKG_PINNED_COMMIT}
            WORKING_DIRECTORY ${VCPKG_ROOT}
            RESULT_VARIABLE VCPKG_CHECKOUT_RESULT
        )
        if(NOT VCPKG_CHECKOUT_RESULT EQUAL 0)
            # Commit not present locally (e.g. the pin moved forward
            # since the clone): fetch it explicitly, then re-checkout.
            execute_process(COMMAND git fetch origin ${VCPKG_PINNED_COMMIT} WORKING_DIRECTORY ${VCPKG_ROOT})
            execute_process(
                COMMAND git -c advice.detachedHead=false checkout ${VCPKG_PINNED_COMMIT}
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE VCPKG_CHECKOUT_RESULT
            )
        endif()
        if(NOT VCPKG_CHECKOUT_RESULT EQUAL 0)
            message(FATAL_ERROR "***** FATAL ERROR: Could not pin vcpkg to ${VCPKG_PINNED_COMMIT} *****")
        endif()
    endif()
endif()

set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)

if(NOT EXISTS ${VCPKG_EXEC})
    message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_EXEC})
    message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
endif()

if(DEFINED CMAKE_GENERATOR_PLATFORM AND CMAKE_GENERATOR_PLATFORM)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "")
    else()
        set(VCPKG_TARGET_TRIPLET "${CMAKE_GENERATOR_PLATFORM}-windows" CACHE STRING "")
    endif()
else()
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "" )
endif()

# Dependencies (GTest, fmt, spdlog) are declared in vcpkg.json and installed
# automatically by the vcpkg toolchain (included below via each preset's
# toolchain.cmake) as part of manifest-mode dependency resolution -- no
# manual `vcpkg install` step needed.

project(IISNativeModuleRoot LANGUAGES C CXX)

# Enforce C++20 for C++ targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Binary hardening ---
# The module DLL is network-facing native code inside w3wp.exe, so it
# gets Control Flow Guard (compile+link; supported by both cl and
# clang-cl) and /sdl (extra compile-time and runtime security checks;
# MSVC-only — clang-cl doesn't implement it). Applied globally so every
# translation unit linked into the DLL is instrumented; /GS, ASLR, and
# DEP are already MSVC defaults.
add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/sdl>"
    "/guard:cf"
)
add_link_options("/GUARD:CF")

# --- Coverage flags (applied only when ENABLE_COVERAGE is ON) ---
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
        add_link_options(-fprofile-instr-generate)
    else()
        message(FATAL_ERROR
            "Coverage requires Clang/clang-cl. Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(bench)

# --- Static analysis (windows-x86-64-static-analysis preset) ---
# Falls back to the vendored copy in scripts/ if run-clang-tidy isn't found
# alongside clang-tidy.exe (not every VS "C++ Clang tools for Windows"
# component version ships the wrapper script).
find_program(RUN_CLANG_TIDY_EXE
    NAMES run-clang-tidy run-clang-tidy.py
    PATHS "${CMAKE_SOURCE_DIR}/scripts"
)
if(RUN_CLANG_TIDY_EXE)
    # run-clang-tidy is always a Python script (no .exe wrapper), including
    # when it ships extension-less alongside clang-tidy.exe -- Windows can't
    # execute that directly, so a real Python 3 interpreter is required.
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_EXECUTABLE)
        set(RUN_CLANG_TIDY_COMMAND ${Python3_EXECUTABLE} ${RUN_CLANG_TIDY_EXE})
    else()
        message(WARNING
            "Found ${RUN_CLANG_TIDY_EXE} but no Python 3 interpreter -- "
            "the 'tidy' target will fail to run. Install Python 3 and ensure "
            "it's on PATH.")
        set(RUN_CLANG_TIDY_COMMAND ${RUN_CLANG_TIDY_EXE})
    endif()
    # -export-fixes points at a directory (one .yaml per file) rather than a
    # single merged file: merging into one file requires PyYAML, which isn't
    # a project dependency. run-clang-tidy only recognizes an *existing*
    # directory here (its own "create if it ends in a separator" check looks
    # for a trailing os.path.sep, i.e. "\", which a CMake "/"-style path
    # never has on Windows), so it's created up front.
    add_custom_target(tidy
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/tidy-fixes"
        COMMAND ${RUN_CLANG_TIDY_COMMAND} -p "${CMAKE_BINARY_DIR}" -export-fixes "${CMAKE_BINARY_DIR}/tidy-fixes"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy static analysis"
    )
endif()

