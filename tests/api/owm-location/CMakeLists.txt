# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(owm_location_parsing_tests)

set(owm_location_parsing_tests_sources
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/NLohmannJsonOwm.cpp
    ../../../src/json/SimdJsonOwm.cpp
    ../../../third-party/simdjson/simdjson.cpp
    main.cpp)

add_definitions(-std=c++17 -Dwic_no_json_parsing -Dwic_owm_find_location -Dwic_weather_comparison)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(owm_location_parsing_tests ${owm_location_parsing_tests_sources})


# test run
add_test(NAME OpenWeatherMap-location-parsing
         COMMAND $<TARGET_FILE:owm_location_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/owm.location.json")
