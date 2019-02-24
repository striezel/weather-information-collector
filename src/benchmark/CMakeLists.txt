# Might work with earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weather-information-collector-benchmark)

set(weather-information-collector-benchmark_sources
    ../api/Types.cpp
    ../conf/Configuration.cpp
    ../data/Forecast.cpp
    ../data/Location.cpp
    ../data/Weather.cpp
    ../db/ConnectionInformation.cpp
    ../db/Utilities.cpp
    ../json/JsonCppDarkSky.cpp
    ../json/JsonCppOwm.cpp
    ../json/NLohmannJsonDarkSky.cpp
    ../json/NLohmannJsonOwm.cpp
    ../retrieve/SourceMySQL.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/Strings.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++14 -Dwic_no_tasks_in_config -Dwic_weather_comparison)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather-information-collector-benchmark ${weather-information-collector-benchmark_sources})

# find jsoncpp library
set(LibJsonCpp_DIR "../../cmake/" )
find_package (LibJsonCpp)
if (LIBJSONCPP_FOUND)
  include_directories(${LIBJSONCPP_INCLUDE_DIRS})
  target_link_libraries (weather-information-collector-benchmark ${LIBJSONCPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libjsoncpp was not found!" )
endif (LIBJSONCPP_FOUND)

# libmysql++
set(LibMySQLPP_DIR "../../cmake/" )
find_package(LibMySQLPP)
if (MYSQLPP_FOUND)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  target_link_libraries (weather-information-collector-benchmark ${MYSQLPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libmysqlpp was not found!" )
endif (MYSQLPP_FOUND)

# find libmysqlclient
set(mysqlclient_DIR "../../cmake/" )
find_package(mysqlclient REQUIRED)
if (MYSQLCLIENT_FOUND)
  include_directories(${MYSQLCLIENT_INCLUDE_DIR})
  target_link_libraries (weather-information-collector-benchmark ${MYSQLCLIENT_LIBRARIES})
else()
  message ( FATAL_ERROR "libmysqlclient was not found!" )
endif (MYSQLCLIENT_FOUND)

# find Boost library: filesystem (+ system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(weather-information-collector-benchmark ${Boost_LIBRARIES})
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
