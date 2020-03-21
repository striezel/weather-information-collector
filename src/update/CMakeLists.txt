# Might work with earlier versions, too, but 2.8 is a reasonable minimum.
cmake_minimum_required (VERSION 2.8)

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
    ../db/mysqlpp/API.cpp
    ../db/mysqlpp/Structure.cpp
    ../db/mysqlpp/guess.cpp
    ../json/ApixuFunctions.cpp
    ../json/NLohmannJsonApixu.cpp
    ../json/NLohmannJsonDarkSky.cpp
    ../json/NLohmannJsonOwm.cpp
    ../json/NLohmannJsonWeatherbit.cpp
    ../json/NLohmannJsonWeatherstack.cpp
    ../json/WeatherstackFunctions.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/SemVer.cpp
    ../util/Strings.cpp
    ForecastDataUpdate_0.8.3.cpp
    Update_0.5.4_to_0.5.5.cpp
    Update_0.5.7_to_0.6.0.cpp
    Update_0.6.5_to_0.6.6.cpp
    UpdateTo_0.7.0.cpp
    UpdateTo_0.8.0.cpp
    UpdateTo_0.8.1.cpp
    UpdateTo_0.8.3.cpp
    WeatherDataUpdate_0.8.3.cpp
    UpdateTo_0.8.5.cpp
    UpdateTo_0.8.6.cpp
    UpdateTo_0.9.0.cpp
    UpdateTo_0.9.7.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++14 -pthread -Dwic_no_network_requests)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s -pthread" )

add_executable(weather-information-collector-update ${weather-information-collector-update_sources})

# libmysql++
set(LibMySQLPP_DIR "../../cmake/" )
find_package(LibMySQLPP)
if (MYSQLPP_FOUND)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  target_link_libraries (weather-information-collector-update ${MYSQLPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libmysqlpp was not found!" )
endif (MYSQLPP_FOUND)

# find Boost library: filesystem (+ system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(weather-information-collector-update ${Boost_LIBRARIES})
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
