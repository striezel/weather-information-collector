cmake_minimum_required (VERSION 3.8)

project(owm_parsing_tests)

set(owm_parsing_tests_sources
    ../../../src/api/OpenWeatherMap.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/NLohmannJsonOwm.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/simdjson-compile-test
            owm-parse-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND owm_parsing_tests_sources
    ../../../third-party/simdjson/simdjson.cpp
    ../../../src/json/SimdJsonOwm.cpp)
  message(STATUS "owm_parsing_tests can use simdjson.")
else()
  list(APPEND owm_parsing_tests_sources
    ../../../src/json/NLohmannJsonOwm.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in owm_parsing_tests.")
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

add_executable(owm_parsing_tests ${owm_parsing_tests_sources})


# test run
add_test(NAME OpenWeatherMap-parsing
         COMMAND $<TARGET_FILE:owm_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/owm.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/owm.forecast.json" "${CMAKE_CURRENT_SOURCE_DIR}/owm.forecast.snow.json")
