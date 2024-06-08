cmake_minimum_required (VERSION 3.8)

project(simdjson_parsing_tests)

set(simdjson_parsing_tests_sources
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/ApixuFunctions.cpp
    ../../../src/json/OpenMeteoFunctions.cpp
    ../../../src/json/SimdJsonApixu.cpp
    ../../../src/json/SimdJsonDarkSky.cpp
    ../../../src/json/SimdJsonOpenMeteo.cpp
    ../../../src/json/SimdJsonOwm.cpp
    ../../../src/json/SimdJsonWeatherbit.cpp
    ../../../src/json/SimdJsonWeatherstack.cpp
    ../../../src/json/WeatherstackFunctions.cpp
    ../../../third-party/simdjson/simdjson.cpp
    SimdJsonApixu.cpp
    SimdJsonDarkSky.cpp
    SimdJsonOpenMeteo.cpp
    SimdJsonOwm.cpp
    SimdJsonWeatherbit.cpp
    SimdJsonWeatherstack.cpp
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

add_definitions(-Dwic_owm_find_location -Dwic_openmeteo_find_location)

add_executable(simdjson_parsing_tests ${simdjson_parsing_tests_sources})

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(simdjson_parsing_tests Catch2::Catch2WithMain)
endif ()


# test run
add_test(NAME simdjson_parsing_error_handling
         COMMAND $<TARGET_FILE:simdjson_parsing_tests>)
