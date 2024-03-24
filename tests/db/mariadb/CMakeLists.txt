cmake_minimum_required (VERSION 3.8)

project(db_tests_mariadb)

set(db_tests_mariadb_sources
    ../../../src/api/Types.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/ForecastMeta.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/data/WeatherMeta.cpp
    ../../../src/db/ConnectionInformation.cpp
    ../../../src/db/Exceptions.cpp
    ../../../src/db/mariadb/API.cpp
    ../../../src/db/mariadb/Connection.cpp
    ../../../src/db/mariadb/Field.cpp
    ../../../src/db/mariadb/Result.cpp
    ../../../src/db/mariadb/Row.cpp
    ../../../src/db/mariadb/SourceMariaDB.cpp
    ../../../src/db/mariadb/StoreMariaDB.cpp
    ../../../src/db/mariadb/StoreMariaDBBatch.cpp
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
    SourceMariaDB.cpp
    StoreMariaDB.cpp
    StoreMariaDBBatch.cpp
    Structure.cpp
    Utilities.cpp
    guess.cpp
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
add_definitions(-Dwic_weather_comparison)

add_executable(db_tests_mariadb ${db_tests_mariadb_sources})

# find libmariadb
find_package(PkgConfig)
pkg_search_module (MARIADB REQUIRED libmariadb)
if (MARIADB_FOUND)
  include_directories(${MARIADB_INCLUDE_DIR})
  target_link_libraries (db_tests_mariadb ${MARIADB_LIBRARIES})
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

# MSYS2 / MinGW uses Catch 3.x.
if (HAS_CATCH_V3)
    find_package(Catch2 3 REQUIRED)
    target_link_libraries(db_tests_mariadb Catch2::Catch2WithMain)
endif ()

# test run
add_test(NAME db_tests_mariadb
        COMMAND $<TARGET_FILE:db_tests_mariadb>)
