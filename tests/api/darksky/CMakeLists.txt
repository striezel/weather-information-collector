# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(darksky_parsing_tests)

set(darksky_parsing_tests_sources
    ../../../src/api/DarkSky.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/NLohmannJsonDarkSky.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

add_definitions(-std=c++14 -Dwic_no_network_requests)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(darksky_parsing_tests ${darksky_parsing_tests_sources})


# test run
add_test(NAME DarkSky-parsing
         COMMAND $<TARGET_FILE:darksky_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/darksky.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/darksky.forecast.json" "${CMAKE_CURRENT_SOURCE_DIR}/darksky.snow.json")
