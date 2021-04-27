# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(owm_location_parsing_tests)

set(owm_location_parsing_tests_sources
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/NLohmannJsonOwm.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/simdjson-compile-test
            owm-location-test-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND owm_location_parsing_tests_sources
    ../../../third-party/simdjson/simdjson.cpp
    ../../../src/json/SimdJsonOwm.cpp)
  message(STATUS "OpenWeatherMap location parsing test can use simdjson.")
else()
  message(STATUS "Compiler is not fit for simdjson, won't use it in owm-location.")
endif(CAN_HAVE_SIMDJSON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Dwic_no_json_parsing -Dwic_owm_find_location -Dwic_weather_comparison)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -O3 -fexceptions)
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(owm_location_parsing_tests ${owm_location_parsing_tests_sources})


# test run
add_test(NAME OpenWeatherMap-location-parsing
         COMMAND $<TARGET_FILE:owm_location_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/owm.location.json")
