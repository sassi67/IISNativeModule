set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "x86")

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Force 32-bit code generation from the (native x86_64) gcc/g++ toolchain.
# Requires the gcc-multilib/g++-multilib packages to be installed.
set(CMAKE_C_FLAGS_INIT "-m32")
set(CMAKE_CXX_FLAGS_INIT "-m32")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-m32")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-m32")

if(NOT DEFINED ENV{VCPKG_ROOT})
    get_filename_component(MODERNCPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(VCPKG_ROOT ${MODERNCPP_SOURCE_DIR}/.vcpkg)
else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
