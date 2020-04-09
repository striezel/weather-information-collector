# Might work with earlier versions, too, but it has not been tested with those.
cmake_minimum_required (VERSION 2.8)

project(db_tests_mariadb)

set(db_tests_mariadb_sources
    ../../../src/api/Types.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/ForecastMeta.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/data/WeatherMeta.cpp
    ../../../src/db/ConnectionInformation.cpp
    ../../../src/db/mariadb/API.cpp
    ../../../src/db/mariadb/Connection.cpp
    ../../../src/db/mariadb/Field.cpp
    ../../../src/db/mariadb/Result.cpp
    ../../../src/db/mariadb/Row.cpp
    ../../../src/db/mariadb/SourceMySQL.cpp
    ../../../src/db/mariadb/StoreMySQL.cpp
    ../../../src/db/mariadb/Structure.cpp
    ../../../src/db/mariadb/Utilities.cpp
    ../../../src/db/mariadb/guess.cpp
    ../../../src/util/Environment.cpp
    ../../../src/util/SemVer.cpp
    ../../../src/util/Strings.cpp
    ../CiConnection.cpp
    ../CiData.cpp
    API.cpp
    Connection.cpp
    Field.cpp
    InitDB.cpp
    Result.cpp
    Row.cpp
    SourceMySQL.cpp
    StoreMySQL.cpp
    Structure.cpp
    Utilities.cpp
    guess.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++14 -Dwic_weather_comparison)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(db_tests_mariadb ${db_tests_mariadb_sources})

# find libmariadb
set(mariadb_DIR "../../../cmake/" )
find_package(mariadb REQUIRED)
if (MARIADB_FOUND)
  include_directories(${MARIADB_INCLUDE_DIR})
  target_link_libraries (db_tests_mariadb ${MARIADB_LIBRARIES})
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

# test run
add_test(NAME db_tests_mariadb
        COMMAND $<TARGET_FILE:db_tests_mariadb>)
