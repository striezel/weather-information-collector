# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(darksky_parsing_tests)

set(darksky_parsing_tests_sources
    ../../../src/api/DarkSky.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/simdjson-compile-test
            darksky-parse-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND darksky_parsing_tests_sources
    ../../../third-party/simdjson/simdjson.cpp
    ../../../src/json/SimdJsonDarkSky.cpp)
  message(STATUS "darksky_parsing_tests can use simdjson.")
else()
  list(APPEND darksky_parsing_tests_sources
    ../../../src/json/NLohmannJsonDarkSky.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in darksky_parsing_tests.")
endif(CAN_HAVE_SIMDJSON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Dwic_no_network_requests)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(darksky_parsing_tests ${darksky_parsing_tests_sources})


# test run
add_test(NAME DarkSky-parsing
         COMMAND $<TARGET_FILE:darksky_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/darksky.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/darksky.forecast.json" "${CMAKE_CURRENT_SOURCE_DIR}/darksky.snow.json")
