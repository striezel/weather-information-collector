# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weatherstack_parsing_tests)

set(weatherstack_parsing_tests_sources
    ../../../src/api/Weatherstack.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/NLohmannJsonWeatherstack.cpp
    ../../../src/json/WeatherstackFunctions.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/simdjson-compile-test
            weatherstack-parse-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND weatherstack_parsing_tests_sources
    ../../../third-party/simdjson/simdjson.cpp
    ../../../src/json/SimdJsonWeatherstack.cpp)
  message(STATUS "weatherstack_parsing_tests can use simdjson.")
else()
  list(APPEND weatherstack_parsing_tests_sources
    ../../../src/json/NLohmannJsonWeatherstack.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in weatherstack_parsing_tests.")
endif(CAN_HAVE_SIMDJSON)

add_definitions(-std=c++17 -Dwic_no_network_requests)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weatherstack_parsing_tests ${weatherstack_parsing_tests_sources})


# test run
add_test(NAME Weatherstack-parsing
         COMMAND $<TARGET_FILE:weatherstack_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/weatherstack.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherstack.current.snow.json")
