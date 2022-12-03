# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(apixu_parsing_tests)

set(apixu_parsing_tests_sources
    ../../../src/api/Apixu.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/ApixuFunctions.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/simdjson-compile-test
            apixu-parse-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND apixu_parsing_tests_sources
    ../../../third-party/simdjson/simdjson.cpp
    ../../../src/json/SimdJsonApixu.cpp)
  message(STATUS "apixu-parsing-tests can use simdjson.")
else()
  list(APPEND apixu_parsing_tests_sources
    ../../../src/json/NLohmannJsonApixu.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in apixu-parsing-tests.")
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

add_executable(apixu_parsing_tests ${apixu_parsing_tests_sources})


# test run
add_test(NAME Apixu-parsing
         COMMAND $<TARGET_FILE:apixu_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/apixu.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.forecast.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.forecast.hour.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.current.snow.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.forecast.snow.json")
