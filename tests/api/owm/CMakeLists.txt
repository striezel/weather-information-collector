# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(owm_parsing_tests)

set(owm_parsing_tests_sources
    ../../../src/api/OpenWeatherMap.cpp
    ../../../src/data/Forecast.cpp
    ../../../src/data/Location.cpp
    ../../../src/data/Weather.cpp
    ../../../src/json/JsonCppOwm.cpp
    ../../../src/net/Curly.cpp
    ../../../src/util/Strings.cpp
    main.cpp)

add_definitions(-std=c++11)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions -std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(owm_parsing_tests ${owm_parsing_tests_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (owm_parsing_tests ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)


# find jsoncpp library
set(LibJsonCpp_DIR "../../../cmake/" )
find_package (LibJsonCpp)
if (LIBJSONCPP_FOUND)
  include_directories(${LIBJSONCPP_INCLUDE_DIRS})
  target_link_libraries (owm_parsing_tests ${LIBJSONCPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libjsoncpp was not found!" )
endif (LIBJSONCPP_FOUND)


# test run
add_test(NAME OpenWeatherMap-parsing
         COMMAND $<TARGET_FILE:owm_parsing_tests> "${CMAKE_CURRENT_SOURCE_DIR}/owm.current.json" "${CMAKE_CURRENT_SOURCE_DIR}/owm.forecast.json" "${CMAKE_CURRENT_SOURCE_DIR}/owm.forecast.snow.json")
