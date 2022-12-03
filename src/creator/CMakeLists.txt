# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(weather-information-collector-task-creator)

set(weather-information-collector-task-creator_sources
    ../api/Apixu.cpp
    ../api/DarkSky.cpp
    ../api/Factory.cpp
    ../api/Limit.cpp
    ../api/Limits.cpp
    ../api/OpenMeteo.cpp
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
    ../json/OpenMeteoFunctions.cpp
    ../net/Curly.cpp
    ../net/Request.cpp
    ../net/curl_version.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/Strings.cpp
    ../util/encode.cpp
    ../Version.cpp
    CliUtilities.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../simdjson-compile-test
            creator-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND weather-information-collector-task-creator_sources
    ../../third-party/simdjson/simdjson.cpp
    ../json/SimdJsonOpenMeteo.cpp
    ../json/SimdJsonOwm.cpp)
  message(STATUS "creator can use simdjson.")
else()
  list(APPEND weather-information-collector-task-creator_sources
    ../json/NLohmannJsonOpenMeteo.cpp
    ../json/NLohmannJsonOwm.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in creator.")
endif(CAN_HAVE_SIMDJSON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -Wextra -Wpedantic -pedantic-errors -Wshadow -fexceptions)
    if (CODE_COVERAGE)
        add_definitions (-O0)
    else ()
        add_definitions (-O3)
    endif ()

    set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# We can safely add the -Dwic_no_network_requests to remove unnecessary methods
# from the API classes, because -Dwic_owm_find_location takes care of all the
# cURL-related stuff, so we can still do web requests for everything we need.
add_definitions(-Dwic_owm_find_location -Dwic_openmeteo_find_location -Dwic_no_network_requests -Dwic_no_json_parsing)

add_executable(weather-information-collector-task-creator ${weather-information-collector-task-creator_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (weather-information-collector-task-creator ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(weather-information-collector-task-creator stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries(weather-information-collector-task-creator c++fs)
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
