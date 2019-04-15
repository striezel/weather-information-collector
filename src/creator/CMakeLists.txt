# Might work with earlier versions, too, but it's not tested.
cmake_minimum_required (VERSION 2.8)

project(weather-information-collector-task-creator)

set(weather-information-collector-task-creator_sources
    ../api/Apixu.cpp
    ../api/DarkSky.cpp
    ../api/Factory.cpp
    ../api/Limits.cpp
    ../api/OpenWeatherMap.cpp
    ../api/Types.cpp
    ../api/Weatherbit.cpp
    ../conf/Configuration.cpp
    ../data/Forecast.cpp
    ../data/Location.cpp
    ../data/Weather.cpp
    ../db/ConnectionInformation.cpp
    ../json/NLohmannJsonOwm.cpp
    ../net/Curly.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/Strings.cpp
    CliUtilities.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
# We can safely add the -Dwic_no_network_requests to remove unnecessary methods
# from the API classes, because -Dwic_owm_find_location takes care of all the
# cURL-related stuff, so we can still do web requests for everything we need.
add_definitions(-std=c++14 -Dwic_owm_find_location -Dwic_no_network_requests -Dwic_no_json_parsing)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather-information-collector-task-creator ${weather-information-collector-task-creator_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (weather-information-collector-task-creator ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# find Boost library: filesystem (+ system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(weather-information-collector-task-creator ${Boost_LIBRARIES})
else ()
  message ( FATAL_ERROR "Boost library was not found!" )
endif (Boost_FOUND)

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
