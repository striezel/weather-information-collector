# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(task_manager_tests)

set(task_manager_tests_sources
    ../../src/api/Apixu.cpp
    ../../src/api/DarkSky.cpp
    ../../src/api/Factory.cpp
    ../../src/api/Limit.cpp
    ../../src/api/Limits.cpp
    ../../src/api/OpenWeatherMap.cpp
    ../../src/api/Types.cpp
    ../../src/api/Weatherbit.cpp
    ../../src/api/Weatherstack.cpp
    ../../src/data/Forecast.cpp
    ../../src/data/Location.cpp
    ../../src/data/Weather.cpp
    ../../src/tasks/Task.cpp
    ../../src/tasks/TaskManager.cpp
    ../../src/util/Strings.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-O3 -Dwic_no_json_parsing -Dwic_no_network_requests)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Weffc++ -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(task_manager_tests ${task_manager_tests_sources})

if (NOT MSVC)
  # link static library for std::filesystem on GNU C++ compiler
  target_link_libraries(task_manager_tests stdc++fs)
endif (NOT MSVC)

# test run
add_test(NAME TaskManager_tests
         COMMAND $<TARGET_FILE:task_manager_tests> "${CMAKE_CURRENT_SOURCE_DIR}")
