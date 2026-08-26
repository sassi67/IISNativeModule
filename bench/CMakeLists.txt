cmake_minimum_required(VERSION 4.2)

project(IISNativeModuleBench C CXX)

file(GLOB BENCH_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Links IISNativeModuleCore to measure the real core::ModuleHandler
# (variant A) against the template and zero-copy variants. Not registered
# with CTest: this is a measurement tool, not a test.
add_executable(${PROJECT_NAME} ${BENCH_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE IISNativeModuleCore)
