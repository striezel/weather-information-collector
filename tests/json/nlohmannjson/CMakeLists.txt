cmake_minimum_required (VERSION 3.8)

project(nlohmannjson_parsing_tests)

set(nlohmannjson_parsing_tests_sources
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/ApixuFunctions.cpp
    ../../../src/json/NLohmannJsonApixu.cpp
    ../../../src/json/NLohmannJsonDarkSky.cpp
    ../../../src/json/NLohmannJsonOwm.cpp
    ../../../src/json/NLohmannJsonWeatherbit.cpp
    ../../../src/json/NLohmannJsonWeatherstack.cpp
    ../../../src/json/WeatherstackFunctions.cpp
    ../../../third-party/nlohmann/json.hpp
    NLohmannJsonApixu.cpp
    NLohmannJsonDarkSky.cpp
    NLohmannJsonOwm.cpp
    NLohmannJsonWeatherbit.cpp
    NLohmannJsonWeatherstack.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Wextra -pedantic -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()

add_definitions(-Dwic_owm_find_location)

add_executable(nlohmannjson_parsing_tests ${nlohmannjson_parsing_tests_sources})

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(nlohmannjson_parsing_tests Catch2::Catch2WithMain)
endif ()


# test run
add_test(NAME nlohmannjson_parsing_error_handling
         COMMAND $<TARGET_FILE:nlohmannjson_parsing_tests>)
