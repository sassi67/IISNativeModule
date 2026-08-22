cmake_minimum_required(VERSION 4.2)

project(IISNativeModuleCore C CXX)

file(GLOB CORE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(${PROJECT_NAME} STATIC ${CORE_FILES})

# Lets consumers include the headers as <core/...>.
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
