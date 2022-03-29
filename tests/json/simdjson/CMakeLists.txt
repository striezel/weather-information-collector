# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(simdjson_parsing_tests)

set(simdjson_parsing_tests_sources
    ../../../src/data/Forecast.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/ApixuFunctions.cpp
    ../../../src/json/SimdJsonApixu.cpp
    ../../../src/json/SimdJsonDarkSky.cpp
    ../../../src/json/SimdJsonOwm.cpp
    ../../../src/json/SimdJsonWeatherbit.cpp
    ../../../src/json/SimdJsonWeatherstack.cpp
    ../../../src/json/WeatherstackFunctions.cpp
    ../../../third-party/simdjson/simdjson.cpp
    SimdJsonApixu.cpp
    SimdJsonDarkSky.cpp
    SimdJsonOwm.cpp
    SimdJsonWeatherbit.cpp
    SimdJsonWeatherstack.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(simdjson_parsing_tests ${simdjson_parsing_tests_sources})


# test run
add_test(NAME simdjson_parsing_error_handling
         COMMAND $<TARGET_FILE:simdjson_parsing_tests>)
