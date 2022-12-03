cmake_minimum_required (VERSION 3.8)

project(generic_json_tests)

set(generic_json_tests_sources
    ../../../src/data/Weather.cpp
    ../../../src/json/ApixuFunctions.cpp
    ../../../src/json/OpenMeteoFunctions.cpp
    ../../../src/json/WeatherstackFunctions.cpp
    ApixuFunctions.cpp
    OpenMeteoFunctions.cpp
    WeatherstackFunctions.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()

add_executable(generic_json_tests ${generic_json_tests_sources})

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(generic_json_tests Catch2::Catch2WithMain)
endif ()


# test run
add_test(NAME generic_json_functions
         COMMAND $<TARGET_FILE:generic_json_tests>)
