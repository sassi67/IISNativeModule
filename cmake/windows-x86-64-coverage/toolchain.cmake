set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "x86_64")

# clang-cl bundled with Visual Studio 2026 Community (VC\Tools\Llvm\x64\bin).
# Requires the "C++ Clang tools for Windows" VS component to be installed.
set(CMAKE_C_COMPILER "$ENV{ProgramFiles}/Microsoft Visual Studio/18/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe" CACHE STRING "")
set(CMAKE_CXX_COMPILER "$ENV{ProgramFiles}/Microsoft Visual Studio/18/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe" CACHE STRING "")

if(NOT DEFINED ENV{VCPKG_ROOT})
    get_filename_component(IISNATIVEMODULE_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(VCPKG_ROOT ${IISNATIVEMODULE_SOURCE_DIR}/.vcpkg)
else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
