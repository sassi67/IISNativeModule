cmake_minimum_required(VERSION 4.2)

# if the environment variable VCPKG_ROOT is not set
# default to the local .vcpkg directory and clone vcpkg if it doesn't exist
if(NOT DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.vcpkg)
else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_ROOT})
    message("Cloning vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
else()
    # The following command has no effect if the vcpkg repository is in a detached head state.
    message("Auto-updating vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_ROOT}/README.md)
    message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
endif()

if(WIN32)
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
    set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
else()
    set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
    set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
endif()

if(NOT EXISTS ${VCPKG_EXEC})
    message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
    execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_EXEC})
    message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
endif()

if(WIN32)
    if(DEFINED CMAKE_GENERATOR_PLATFORM AND CMAKE_GENERATOR_PLATFORM)
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
            set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "")
        else()
            set(VCPKG_TARGET_TRIPLET "${CMAKE_GENERATOR_PLATFORM}-windows" CACHE STRING "")
        endif()
    else()
        set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "" )
    endif()
elseif(LINUX)
    # CMAKE_SYSTEM_PROCESSOR isn't populated until the toolchain file runs during
    # project(), which is too late to pick the triplet vcpkg's manifest-mode
    # install resolves against, so infer the target architecture from the
    # toolchain file path instead.
    if(CMAKE_TOOLCHAIN_FILE MATCHES "linux-x86-32")
        set(VCPKG_TARGET_TRIPLET "x86-linux" CACHE STRING "")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
    endif()
endif()

# Dependencies (GTest, fmt, spdlog) are declared in vcpkg.json and installed
# automatically by the vcpkg toolchain (included below via each preset's
# toolchain.cmake) as part of manifest-mode dependency resolution -- no
# manual `vcpkg install` step needed.

project(ModernCppRoot LANGUAGES C CXX)

# Enforce C++20 for C++ targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# --- Static analysis (windows/linux -x86-64-static-analysis presets) ---
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

