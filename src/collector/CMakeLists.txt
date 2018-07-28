# Might work with earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weather-information-collector)

set(weather-information-collector_sources
    ../api/Apixu.cpp
    ../api/DarkSky.cpp
    ../api/Factory.cpp
    ../api/Limits.cpp
    ../api/OpenWeatherMap.cpp
    ../api/Types.cpp
    ../conf/Configuration.cpp
    ../data/Location.cpp
    ../data/Weather.cpp
    ../db/ConnectionInformation.cpp
    ../db/Utilities.cpp
    ../net/Curly.cpp
    ../store/StoreCSV.cpp
    ../store/StoreMySQL.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/GitInfos.cpp
    ../util/Strings.cpp
    Collector.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++11)

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

# find jsoncpp library
set(LibJsonCpp_DIR "../../cmake/" )
find_package (LibJsonCpp)
if (LIBJSONCPP_FOUND)
  include_directories(${LIBJSONCPP_INCLUDE_DIRS})
  target_link_libraries (weather-information-collector ${LIBJSONCPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libjsoncpp was not found!" )
endif (LIBJSONCPP_FOUND)

# libmysql++
set(LibMySQLPP_DIR "../../cmake/" )
find_package(LibMySQLPP)
if (MYSQLPP_FOUND)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  target_link_libraries (weather-information-collector ${MYSQLPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libmysqlpp was not found!" )
endif (MYSQLPP_FOUND)

# find libmysqlclient
set(mysqlclient_DIR "../../cmake/" )
find_package(mysqlclient REQUIRED)
if (MYSQLCLIENT_FOUND)
  include_directories(${MYSQLCLIENT_INCLUDE_DIR})
  target_link_libraries (weather-information-collector ${MYSQLCLIENT_LIBRARIES})
else()
  message ( FATAL_ERROR "libmysqlclient was not found!" )
endif (MYSQLCLIENT_FOUND)

# find Boost library: filesystem (+ system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(weather-information-collector ${Boost_LIBRARIES})
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
