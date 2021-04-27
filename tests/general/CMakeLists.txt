# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(weather_information_collector_tests)

set(weather_information_collector_tests_sources
    ../../src/api/Apixu.cpp
    ../../src/api/DarkSky.cpp
    ../../src/api/Factory.cpp
    ../../src/api/Limit.cpp
    ../../src/api/Limits.cpp
    ../../src/api/OpenWeatherMap.cpp
    ../../src/api/Plans.cpp
    ../../src/api/Types.cpp
    ../../src/api/Weatherbit.cpp
    ../../src/api/Weatherstack.cpp
    ../../src/conf/Configuration.cpp
    ../../src/data/Forecast.cpp
    ../../src/data/Location.cpp
    ../../src/data/Weather.cpp
    ../../src/db/ConnectionInformation.cpp
    ../../src/tasks/Task.cpp
    ../../src/tasks/TaskManager.cpp
    ../../src/util/Directories.cpp
    ../../src/util/SemVer.cpp
    ../../src/util/Strings.cpp
    api/Factory.cpp
    api/Plans.cpp
    api/SupportedDataTypes.cpp
    api/Types.cpp
    conf/Configuration.cpp
    data/Location.cpp
    data/Weather.cpp
    db/ConnectionInformation.cpp
    tasks/Plans.cpp
    tasks/Task.cpp
    tasks/TaskManager.cpp
    util/Directories.cpp
    util/NumericPrecision.cpp
    util/SemVer.cpp
    util/Strings.cpp
    main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Dwic_no_json_parsing -Dwic_no_network_requests)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -O3 -fexceptions)
endif ()

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather_information_collector_tests ${weather_information_collector_tests_sources})

if (NOT MSVC)
  # link static library for std::filesystem on GNU C++ compiler
  target_link_libraries(weather_information_collector_tests stdc++fs)
endif (NOT MSVC)

# test run
add_test(NAME test-suite
         COMMAND $<TARGET_FILE:weather_information_collector_tests>)
