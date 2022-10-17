# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(task_manager_tests)

set(task_manager_tests_sources
    ../../src/api/Apixu.cpp
    ../../src/api/DarkSky.cpp
    ../../src/api/Factory.cpp
    ../../src/api/Limit.cpp
    ../../src/api/Limits.cpp
    ../../src/api/OpenMeteo.cpp
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
add_definitions(-Dwic_no_json_parsing -Dwic_no_network_requests)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_definitions(-Wall -Weffc++ -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()
endif ()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -O3 -fexceptions)
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(task_manager_tests ${task_manager_tests_sources})

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(task_manager_tests stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries(task_manager_tests c++fs)
endif ()

# test run
add_test(NAME TaskManager_tests
         COMMAND $<TARGET_FILE:task_manager_tests> "${CMAKE_CURRENT_SOURCE_DIR}")
