# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(apixu_parsing_tests)

set(apixu_parsing_tests_sources
    ../../../src/api/Apixu.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/ApixuFunctions.cpp
    ../../../src/json/NLohmannJsonApixu.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

add_definitions(-std=c++14 -Dwic_no_network_requests)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(apixu_parsing_tests ${apixu_parsing_tests_sources})


# test run
add_test(NAME Apixu-parsing
         COMMAND $<TARGET_FILE:apixu_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/apixu.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.forecast.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.forecast.hour.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.current.snow.json" "${CMAKE_CURRENT_SOURCE_DIR}/apixu.forecast.snow.json")
