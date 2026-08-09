set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "x86_64")

# clang-cl bundled with Visual Studio (VC\Tools\Llvm\x64\bin), located via
# vswhere rather than assumed under %ProgramFiles% -- vswhere.exe always
# installs under Program Files (x86) even when VS itself is installed to a
# different drive/location. Requires the "C++ Clang tools for Windows" VS
# component to be installed.
find_program(MODERNCPP_VSWHERE_EXE vswhere
    PATHS "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/Installer"
)
if(NOT MODERNCPP_VSWHERE_EXE)
    message(FATAL_ERROR "vswhere.exe not found; cannot locate the Visual Studio installation.")
endif()

execute_process(
    COMMAND "${MODERNCPP_VSWHERE_EXE}" -latest -property installationPath
    OUTPUT_VARIABLE MODERNCPP_VS_INSTALL_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(MODERNCPP_CLANG_CL "${MODERNCPP_VS_INSTALL_PATH}/VC/Tools/Llvm/x64/bin/clang-cl.exe")
if(NOT EXISTS "${MODERNCPP_CLANG_CL}")
    message(FATAL_ERROR "clang-cl.exe not found at ${MODERNCPP_CLANG_CL}; install the \"C++ Clang tools for Windows\" VS component.")
endif()

set(CMAKE_C_COMPILER "${MODERNCPP_CLANG_CL}" CACHE STRING "")
set(CMAKE_CXX_COMPILER "${MODERNCPP_CLANG_CL}" CACHE STRING "")

if(NOT DEFINED ENV{VCPKG_ROOT})
    get_filename_component(MODERNCPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(VCPKG_ROOT ${MODERNCPP_SOURCE_DIR}/.vcpkg)
else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
