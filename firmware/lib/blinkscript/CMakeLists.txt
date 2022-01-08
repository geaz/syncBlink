cmake_minimum_required(VERSION 3.12)
project(syncblinkarduinoapitests VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-Catch2.txt)
include(CTest)
include(Catch)

file(GLOB_RECURSE SYNCBLINKARDUINOAPI_TESTS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/printer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/scanner/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/")

add_executable(syncblinkarduinoapitests ${SYNCBLINKARDUINOAPI_TESTS})
target_link_libraries(syncblinkarduinoapitests Catch2::Catch2)
catch_discover_tests(syncblinkarduinoapitests)