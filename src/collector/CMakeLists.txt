cmake_minimum_required (VERSION 3.8...3.31)

project(weather-information-collector)

set(weather-information-collector_sources
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
    ../db/Exceptions.cpp
    ../db/mariadb/client_version.cpp
    ../db/mariadb/Connection.cpp
    ../db/mariadb/Field.cpp
    ../db/mariadb/Result.cpp
    ../db/mariadb/Row.cpp
    ../db/mariadb/StoreMariaDB.cpp
    ../db/mariadb/Structure.cpp
    ../db/mariadb/Utilities.cpp
    ../db/mariadb/guess.cpp
    ../json/ApixuFunctions.cpp
    ../json/OpenMeteoFunctions.cpp
    ../json/WeatherstackFunctions.cpp
    ../net/Curly.cpp
    ../net/Request.cpp
    ../net/curl_version.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/Directories.cpp
    ../util/GitInfos.cpp
    ../util/SemVer.cpp
    ../util/Strings.cpp
    ../Version.cpp
    Collector.cpp
    main.cpp)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../simdjson-compile-test
            collector-simd-test
)

if (CAN_HAVE_SIMDJSON)
  list(APPEND weather-information-collector_sources
    ../../third-party/simdjson/simdjson.cpp
    ../json/SimdJsonApixu.cpp
    ../json/SimdJsonDarkSky.cpp
    ../json/SimdJsonOpenMeteo.cpp
    ../json/SimdJsonOwm.cpp
    ../json/SimdJsonWeatherbit.cpp
    ../json/SimdJsonWeatherstack.cpp)
  message(STATUS "collector can use simdjson.")
else()
  list(APPEND weather-information-collector_sources
    ../json/NLohmannJsonApixu.cpp
    ../json/NLohmannJsonDarkSky.cpp
    ../json/NLohmannJsonOpenMeteo.cpp
    ../json/NLohmannJsonOwm.cpp
    ../json/NLohmannJsonWeatherbit.cpp
    ../json/NLohmannJsonWeatherstack.cpp)
  message(STATUS "Compiler is not fit for simdjson, won't use it in collector.")
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

add_executable(weather-information-collector ${weather-information-collector_sources})

# find cURL library
find_package (PkgConfig)
pkg_search_module (CURL REQUIRED libcurl)
if (CURL_FOUND)
  if (ENABLE_STATIC_LINKING)
    add_definitions (-DCURL_STATICLIB)
    include_directories(${CURL_STATIC_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${CURL_STATIC_LIBRARIES})
    # cares
    pkg_search_module (CARES REQUIRED libcares)
    if (NOT CARES_FOUND)
      message ( FATAL_ERROR "libcares was not found!" )
    endif ()
    include_directories(${CARES_STATIC_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${CARES_STATIC_LIBRARIES})
    # idn2
    pkg_search_module (IDN2 REQUIRED libidn2)
    if (NOT IDN2_FOUND)
      message ( FATAL_ERROR "libidn2 was not found!" )
    endif ()
    include_directories(${IDN2_STATIC_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${IDN2_STATIC_LIBRARIES})
    # brotlidec
    pkg_search_module (BROTLIDEC REQUIRED libbrotlidec)
    if (NOT BROTLIDEC_FOUND)
      message ( FATAL_ERROR "libbrotlidec was not found!" )
    endif ()
    include_directories(${BROTLIDEC_STATIC_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${BROTLIDEC_STATIC_LIBRARIES})
  else ()
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${CURL_LIBRARIES})
  endif ()
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# find libmariadb
pkg_search_module (MARIADB REQUIRED libmariadb)
if (MARIADB_FOUND)
  if (ENABLE_STATIC_LINKING)
    include_directories(${MARIADB_STATIC_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${MARIADB_STATIC_LIBRARIES})
  else ()
    include_directories(${MARIADB_INCLUDE_DIRS})
    target_link_libraries (weather-information-collector ${MARIADB_LIBRARIES})
  endif (ENABLE_STATIC_LINKING)
else()
  message ( FATAL_ERROR "libmariadb was not found!" )
endif (MARIADB_FOUND)

# GNU GCC before 9.1.0 needs to link to libstdc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
  target_link_libraries(weather-information-collector stdc++fs)
endif ()

# Clang before 9.0 needs to link to libc++fs explicitly.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries(weather-information-collector c++fs)
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
