set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "x86")

if(NOT DEFINED ENV{VCPKG_ROOT})
    get_filename_component(MODERNCPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    set(VCPKG_ROOT ${MODERNCPP_SOURCE_DIR}/.vcpkg)
else()
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
