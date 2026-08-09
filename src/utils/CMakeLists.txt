cmake_minimum_required(VERSION 4.2)

project(ModernCppUtils C CXX)

file(GLOB UTILS_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(${PROJECT_NAME} SHARED STATIC ${UTILS_FILES})
