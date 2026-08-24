cmake_minimum_required(VERSION 4.2)

project(IISNativeModuleTest)

file(GLOB TEST_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"
)

add_executable(${PROJECT_NAME} ${TEST_FILES})

# IISNativeModuleCore exports src/ as a PUBLIC include dir, which is
# what makes the <core/...> includes resolve here.
target_link_libraries(${PROJECT_NAME} PRIVATE
    GTest::gtest
    IISNativeModuleCore
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
