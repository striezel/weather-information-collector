# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Dwic_no_network_requests)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()

add_executable(weatherstack_parsing_tests ${weatherstack_parsing_tests_sources})


# test run
add_test(NAME Weatherstack-parsing
         COMMAND $<TARGET_FILE:weatherstack_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/weatherstack.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherstack.current.snow.json")
