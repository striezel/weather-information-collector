# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weatherbit_parsing_tests)

set(weatherbit_parsing_tests_sources
    ../../../src/api/Weatherbit.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/NLohmannJsonWeatherbit.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

add_definitions(-std=c++14 -Dwic_no_network_requests)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weatherbit_parsing_tests ${weatherbit_parsing_tests_sources})


# test run
add_test(NAME Weatherbit-parsing
         COMMAND $<TARGET_FILE:weatherbit_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.forecast.hourly.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.forecast.3hourly.json" "${CMAKE_CURRENT_SOURCE_DIR}/weatherbit.forecast.daily.json")
