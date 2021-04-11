# Might work with earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weather-information-collector)

set(weather-information-collector_sources
    ../api/Apixu.cpp
    ../api/DarkSky.cpp
    ../api/Factory.cpp
    ../api/Limit.cpp
    ../api/Limits.cpp
    ../api/OpenWeatherMap.cpp
    ../api/Plans.cpp
    ../api/Types.cpp
    ../api/Weatherbit.cpp
    ../api/Weatherstack.cpp
    ../conf/Configuration.cpp
    ../data/Forecast.cpp
    ../data/Location.cpp
    ../data/Weather.cpp
    ../db/ConnectionInformation.cpp
    ../db/mariadb/Connection.cpp
    ../db/mariadb/Field.cpp
    ../db/mariadb/Result.cpp
    ../db/mariadb/Row.cpp
    ../db/mariadb/StoreMySQL.cpp
    ../db/mariadb/Structure.cpp
    ../db/mariadb/Utilities.cpp
    ../db/mariadb/guess.cpp
    ../json/ApixuFunctions.cpp
    ../json/WeatherstackFunctions.cpp
    ../net/Curly.cpp
    ../net/Request.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/SemVer.cpp
    ../util/Strings.cpp
    Collector.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../simdjson-compile-test
            collector-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND weather-information-collector_sources
    ../../third-party/simdjson/simdjson.cpp
    ../json/SimdJsonApixu.cpp
    ../json/SimdJsonDarkSky.cpp
    ../json/SimdJsonOwm.cpp
    ../json/SimdJsonWeatherbit.cpp
    ../json/SimdJsonWeatherstack.cpp)
  message(STATUS "collector can use simdjson.")
else()
  list(APPEND weather-information-collector_sources
    ../json/NLohmannJsonApixu.cpp
    ../json/NLohmannJsonDarkSky.cpp
    ../json/NLohmannJsonOwm.cpp
    ../json/NLohmannJsonWeatherbit.cpp
    ../json/NLohmannJsonWeatherstack.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in collector.")
endif(CAN_HAVE_SIMDJSON)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++17)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather-information-collector ${weather-information-collector_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (weather-information-collector ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# find libmariadb
set(mariadb_DIR "../../cmake/" )
find_package(mariadb REQUIRED)
if (MARIADB_FOUND)
  include_directories(${MARIADB_INCLUDE_DIR})
  target_link_libraries (weather-information-collector ${MARIADB_LIBRARIES})
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

if (NOT MSVC)
  # link static library for std::filesystem on GNU C++ compiler
  target_link_libraries(weather-information-collector stdc++fs)
endif ()

# create git-related constants
# -- get the current commit hash
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# -- get the commit date
execute_process(
  COMMAND git show -s --format=%ci
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TIME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("GIT_HASH is ${GIT_HASH}.")
message("GIT_TIME is ${GIT_TIME}.")

# replace git-related constants in GitInfos.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../util/GitInfos.template.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/../util/GitInfos.cpp
               ESCAPE_QUOTES)
