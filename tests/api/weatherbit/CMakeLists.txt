# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(weatherbit_parsing_tests)

set(weatherbit_parsing_tests_sources
    ../../../src/api/Weatherbit.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/simdjson-compile-test
            weatherbit-parse-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND weatherbit_parsing_tests_sources
    ../../../third-party/simdjson/simdjson.cpp
    ../../../src/json/SimdJsonWeatherbit.cpp)
  message(STATUS "weatherbit_parsing_tests can use simdjson.")
else()
  list(APPEND weatherbit_parsing_tests_sources
    ../../../src/json/NLohmannJsonWeatherbit.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in weatherbit_parsing_tests.")
endif(CAN_HAVE_SIMDJSON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Dwic_no_network_requests)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -O3 -fexceptions)
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weatherbit_parsing_tests ${weatherbit_parsing_tests_sources})


# test run
add_test(NAME Weatherbit-parsing
         COMMAND $<TARGET_FILE:weatherbit_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.forecast.hourly.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.forecast.3hourly.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.forecast.daily.json")
