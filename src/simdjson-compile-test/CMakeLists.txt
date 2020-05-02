cmake_minimum_required (VERSION 2.8)

project(simdjson-compile-test)

set(simdjson-compile-test_sources
    ../../third-party/simdjson/simdjson.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O2 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++17 -O2)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(simdjson-compile-test ${simdjson-compile-test_sources})
