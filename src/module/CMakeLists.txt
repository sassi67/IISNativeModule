cmake_minimum_required(VERSION 4.2)

project(IISNativeModule C CXX)

file(GLOB MODULE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# The .def file exports RegisterModule, the entry point IIS looks up
# with GetProcAddress when loading the module.
add_library(${PROJECT_NAME} SHARED
    ${MODULE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/iis_native_module.def"
)
