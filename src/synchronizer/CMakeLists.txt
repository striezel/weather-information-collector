# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

project(weather-information-collector-synchronizer)

set(weather-information-collector-synchronizer_sources
    ../api/Types.cpp
    ../api/Plans.cpp
    ../conf/Configuration.cpp
    ../data/Forecast.cpp
    ../data/ForecastMeta.cpp
    ../data/Location.cpp
    ../data/Weather.cpp
    ../data/WeatherMeta.cpp
    ../db/ConnectionInformation.cpp
    ../db/Exceptions.cpp
    ../db/mariadb/Connection.cpp
    ../db/mariadb/Field.cpp
    ../db/mariadb/Result.cpp
    ../db/mariadb/Row.cpp
    ../db/mariadb/SourceMySQL.cpp
    ../db/mariadb/StoreMySQL.cpp
    ../db/mariadb/StoreMySQLBatch.cpp
    ../db/mariadb/Structure.cpp
    ../db/mariadb/Utilities.cpp
    ../db/mariadb/guess.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/SemVer.cpp
    ../util/Strings.cpp
    ../Version.cpp
    main.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions (-Wall -O3 -fexceptions)
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Dwic_no_tasks_in_config -Dwic_no_json_parsing)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather-information-collector-synchronizer ${weather-information-collector-synchronizer_sources})

# find libmariadb
set(mariadb_DIR "../../cmake/" )
find_package(mariadb REQUIRED)
if (MARIADB_FOUND)
  include_directories(${MARIADB_INCLUDE_DIR})
  target_link_libraries (weather-information-collector-synchronizer ${MARIADB_LIBRARIES})
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

if (NOT MSVC)
  # link static library for std::filesystem on GNU C++ compiler
  target_link_libraries(weather-information-collector-synchronizer stdc++fs)
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
