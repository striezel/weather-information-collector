cmake_minimum_required (VERSION 3.8)

project(weather_information_collector_tests)

set(weather_information_collector_tests_sources
    ../../src/api/Apixu.cpp
    ../../src/api/DarkSky.cpp
    ../../src/api/Factory.cpp
    ../../src/api/Limit.cpp
    ../../src/api/Limits.cpp
    ../../src/api/OpenMeteo.cpp
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
    ../../src/db/Exceptions.cpp
    ../../src/json/ApixuFunctions.cpp
    ../../src/json/OpenMeteoFunctions.cpp
    ../../src/json/WeatherstackFunctions.cpp
    ../../src/net/Curly.cpp
    ../../src/net/Request.cpp
    ../../src/tasks/Task.cpp
    ../../src/tasks/TaskManager.cpp
    ../../src/util/Directories.cpp
    ../../src/util/Environment.cpp
    ../../src/util/SemVer.cpp
    ../../src/util/Strings.cpp
    api/Apixu.cpp
    api/DarkSky.cpp
    api/Factory.cpp
    api/OpenMeteo.cpp
    api/OpenWeatherMap.cpp
    api/Plans.cpp
    api/SupportedDataTypes.cpp
    api/Types.cpp
    api/Weatherbit.cpp
    api/Weatherstack.cpp
    conf/Configuration.cpp
    data/Location.cpp
    data/Weather.cpp
    db/ConnectionInformation.cpp
    db/Exceptions.cpp
    net/Curly.cpp
    net/Request.cpp
    tasks/Plans.cpp
    tasks/Task.cpp
    tasks/TaskManager.cpp
    util/Directories.cpp
    util/Environment.cpp
    util/NumericPrecision.cpp
    util/SemVer.cpp
    util/Strings.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/simdjson-compile-test
            tests-general-simd-test
)
if (CAN_HAVE_SIMDJSON)
  list(APPEND weather_information_collector_tests_sources
    ../../third-party/simdjson/simdjson.cpp
    ../../src/json/SimdJsonApixu.cpp
    ../../src/json/SimdJsonDarkSky.cpp
    ../../src/json/SimdJsonOpenMeteo.cpp
    ../../src/json/SimdJsonOwm.cpp
    ../../src/json/SimdJsonWeatherbit.cpp
    ../../src/json/SimdJsonWeatherstack.cpp)
  message(STATUS "tests (general) can use simdjson.")
else()
  list(APPEND weather_information_collector_tests_sources
    ../../src/json/NLohmannJsonApixu.cpp
    ../../src/json/NLohmannJsonDarkSky.cpp
    ../../src/json/NLohmannJsonOpenMeteo.cpp
    ../../src/json/NLohmannJsonOwm.cpp
    ../../src/json/NLohmannJsonWeatherbit.cpp
    ../../src/json/NLohmannJsonWeatherstack.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in tests.")
endif(CAN_HAVE_SIMDJSON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()

add_executable(weather_information_collector_tests ${weather_information_collector_tests_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (weather_information_collector_tests ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(weather_information_collector_tests stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries(weather_information_collector_tests c++fs)
endif ()

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(weather_information_collector_tests Catch2::Catch2WithMain)
endif ()

# test run
add_test(NAME test-suite
         COMMAND $<TARGET_FILE:weather_information_collector_tests>)
