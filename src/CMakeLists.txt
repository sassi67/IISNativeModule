cmake_minimum_required(VERSION 4.2)

project(IISNativeModuleApp C CXX)

add_subdirectory(core)
add_subdirectory(module)
add_subdirectory(utils)

file(GLOB SRC_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    IISNativeModuleUtils 
    fmt::fmt
    spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE "." )