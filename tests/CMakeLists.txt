# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weather_information_collector_tests)

set(weather_information_collector_tests_sources
    ../src/api/Types.cpp
    ../src/data/Location.cpp
    ../src/data/Weather.cpp
    ../src/util/Strings.cpp
    api/Types.cpp
    data/Location.cpp
    data/Weather.cpp
    util/Strings.cpp
    main.cpp)

add_definitions(-std=c++11)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions -std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather_information_collector_tests ${weather_information_collector_tests_sources})

# test run
add_test(NAME test-suite
         COMMAND $<TARGET_FILE:weather_information_collector_tests>)


# Recurse into subdirectory for Apixu tests.
add_subdirectory (api/apixu)
