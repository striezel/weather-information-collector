# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(task_manager_tests)

set(task_manager_tests_sources
    ../../src/api/Apixu.cpp
    ../../src/api/DarkSky.cpp
    ../../src/api/Factory.cpp
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

add_definitions(-std=c++14 -O3 -Dwic_no_json_parsing -Dwic_no_network_requests)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Weffc++ -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(task_manager_tests ${task_manager_tests_sources})

# find Boost library: filesystem (+system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(task_manager_tests ${Boost_LIBRARIES})
else ()
  message ( FATAL_ERROR "Boost library was not found!" )
endif (Boost_FOUND)


# test run
add_test(NAME TaskManager_tests
         COMMAND $<TARGET_FILE:task_manager_tests> "${CMAKE_CURRENT_SOURCE_DIR}")
