# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(task_manager_tests)

set(task_manager_tests_sources
    ../../../src/api/Types.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/tasks/Task.cpp
    ../../../src/tasks/TaskManager.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

add_definitions(-std=c++11)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions -std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(task_manager_tests ${task_manager_tests_sources})


# test run
add_test(NAME TaskManager_tests
         COMMAND $<TARGET_FILE:task_manager_tests> "${CMAKE_CURRENT_SOURCE_DIR}")
