cmake_minimum_required (VERSION 3.8...3.31)

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
    ../db/mariadb/client_version.cpp
    ../db/mariadb/Connection.cpp
    ../db/mariadb/Field.cpp
    ../db/mariadb/Result.cpp
    ../db/mariadb/Row.cpp
    ../db/mariadb/SourceMariaDB.cpp
    ../db/mariadb/StoreMariaDB.cpp
    ../db/mariadb/StoreMariaDBBatch.cpp
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
add_definitions(-Dwic_no_tasks_in_config -Dwic_no_json_parsing)

add_executable(weather-information-collector-synchronizer ${weather-information-collector-synchronizer_sources})

# find libmariadb
find_package(PkgConfig)
pkg_search_module (MARIADB REQUIRED libmariadb)
if (MARIADB_FOUND)
  if (ENABLE_STATIC_LINKING)
    include_directories(${MARIADB_STATIC_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector-synchronizer ${MARIADB_STATIC_LIBRARIES})
  else ()
    include_directories(${MARIADB_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector-synchronizer ${MARIADB_LIBRARIES})
  endif (ENABLE_STATIC_LINKING)
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(weather-information-collector-synchronizer stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries(weather-information-collector-synchronizer c++fs)
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
