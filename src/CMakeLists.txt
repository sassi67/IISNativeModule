cmake_minimum_required(VERSION 4.2)

project(ModernCppApp C CXX)

add_subdirectory(utils)

file(GLOB SRC_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    ModernCppUtils 
    fmt::fmt
    spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE "." )