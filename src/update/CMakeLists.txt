# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(weather-information-collector-update)

set(weather-information-collector-update_sources
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
    ../db/Exceptions.cpp
    ../db/mariadb/API.cpp
    ../db/mariadb/client_version.cpp
    ../db/mariadb/Connection.cpp
    ../db/mariadb/Field.cpp
    ../db/mariadb/Result.cpp
    ../db/mariadb/Row.cpp
    ../db/mariadb/Structure.cpp
    ../db/mariadb/guess.cpp
    ../json/ApixuFunctions.cpp
    ../json/WeatherstackFunctions.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/SemVer.cpp
    ../util/Strings.cpp
    ../Version.cpp
    ForecastDataUpdate_0.8.3.cpp
    UpdateTo_0.5.5.cpp
    UpdateTo_0.6.0.cpp
    UpdateTo_0.6.6.cpp
    UpdateTo_0.7.0.cpp
    UpdateTo_0.8.0.cpp
    UpdateTo_0.8.1.cpp
    UpdateTo_0.8.3.cpp
    UpdateTo_0.8.5.cpp
    UpdateTo_0.8.6.cpp
    UpdateTo_0.9.0.cpp
    UpdateTo_0.9.7.cpp
    UpdateTo_0.9.17.cpp
    WeatherDataUpdate_0.8.3.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../simdjson-compile-test
            update-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND weather-information-collector-update_sources
    ../../third-party/simdjson/simdjson.cpp
    ../json/SimdJsonApixu.cpp
    ../json/SimdJsonDarkSky.cpp
    ../json/SimdJsonOwm.cpp
    ../json/SimdJsonWeatherbit.cpp
    ../json/SimdJsonWeatherstack.cpp)
  message(STATUS "updater can use simdjson.")
else()
  list(APPEND weather-information-collector-update_sources
    ../json/NLohmannJsonApixu.cpp
    ../json/NLohmannJsonDarkSky.cpp
    ../json/NLohmannJsonOwm.cpp
    ../json/NLohmannJsonWeatherbit.cpp
    ../json/NLohmannJsonWeatherstack.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in updater.")
endif(CAN_HAVE_SIMDJSON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -O3 -fexceptions)
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-pthread -Dwic_no_network_requests)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s -pthread" )

add_executable(weather-information-collector-update ${weather-information-collector-update_sources})

# find libmariadb
set(mariadb_DIR "../../cmake/" )
find_package(mariadb REQUIRED)
if (MARIADB_FOUND)
  include_directories(${MARIADB_INCLUDE_DIR})
  target_link_libraries (weather-information-collector-update ${MARIADB_LIBRARIES})
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(weather-information-collector-update stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries(weather-information-collector-update c++fs)
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
