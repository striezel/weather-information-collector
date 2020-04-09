# Might work with earlier versions, too, but it has not been tested with those.
cmake_minimum_required (VERSION 2.8)

project(db_tests_mysqlpp2)

set(db_tests_mysqlpp2_sources
    ../../../src/api/Types.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/db/ConnectionInformation.cpp
    ../../../src/db/mysqlpp/SourceMySQL.cpp
    ../../../src/db/mysqlpp/StoreMySQL.cpp
    ../../../src/db/mysqlpp/Structure.cpp
    ../../../src/db/mysqlpp/Utilities.cpp
    ../../../src/util/Environment.cpp
    ../../../src/util/Strings.cpp
    ../CiConnection.cpp
    ../CiData.cpp
    ../mysqlpp/InitDB.cpp
    SourceMySQL.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++14 -Dwic_weather_comparison -Dwic_no_metadata)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(db_tests_mysqlpp2 ${db_tests_mysqlpp2_sources})

# libmysql++
set(LibMySQLPP_DIR "../../../cmake/" )
find_package(LibMySQLPP)
if (MYSQLPP_FOUND)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  target_link_libraries (db_tests_mysqlpp2 ${MYSQLPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libmysqlpp was not found!" )
endif (MYSQLPP_FOUND)

# find libmysqlclient
set(mysqlclient_DIR "../../../cmake/" )
find_package(mysqlclient REQUIRED)
if (MYSQLCLIENT_FOUND)
  include_directories(${MYSQLCLIENT_INCLUDE_DIR})
  target_link_libraries (db_tests_mysqlpp2 ${MYSQLCLIENT_LIBRARIES})
else()
  message ( FATAL_ERROR "libmysqlclient was not found!" )
endif (MYSQLCLIENT_FOUND)

# test run
add_test(NAME db_tests_mysqlpp2
        COMMAND $<TARGET_FILE:db_tests_mysqlpp2>)
