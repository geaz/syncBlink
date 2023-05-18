cmake_minimum_required(VERSION 3.12)
#set(CMAKE_GENERATOR_PLATFORM win32) # We want to have the same 32bit architecture as the esp8266 - otherwise the cross compilation of scripts will not work

project(blinkScript VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 17)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/")

if (DEFINED EMSCRIPTEN)
    file(GLOB_RECURSE SYNCBLINKWASM   
        "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/printer/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/program/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/source/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/blink_script_web.cpp"
    )

    add_executable(blinkScript ${SYNCBLINKWASM})
    set_target_properties(blinkScript PROPERTIES COMPILE_FLAGS "-Os")
    set_target_properties(blinkScript PROPERTIES LINK_FLAGS    "-Os -s ENVIRONMENT=web -s SINGLE_FILE=1 -s MODULARIZE -s EXPORT_ES6=1 -s EXPORT_NAME=blinkScript -s ALLOW_MEMORY_GROWTH --bind")
else()
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-Catch2.txt)
    include(CTest)
    include(Catch)

    file(GLOB_RECURSE SYNCBLINKARDUINOAPI_TESTS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/printer/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/program/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/source/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )
    add_executable(blinkScript ${SYNCBLINKARDUINOAPI_TESTS})
    target_link_libraries(blinkScript Catch2::Catch2)
    catch_discover_tests(blinkScript)
endif()