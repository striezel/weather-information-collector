# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weather_information_collector_tests)

set(weather_information_collector_tests_sources
    ../src/api/Apixu.cpp
    ../src/api/DarkSky.cpp
    ../src/api/Factory.cpp
    ../src/api/Limits.cpp
    ../src/api/OpenWeatherMap.cpp
    ../src/api/Types.cpp
    ../src/conf/Configuration.cpp
    ../src/data/Forecast.cpp
    ../src/data/Location.cpp
    ../src/data/Weather.cpp
    ../src/db/ConnectionInformation.cpp
    ../src/json/JsonCppApixu.cpp
    ../src/json/JsonCppDarkSky.cpp
    ../src/json/JsonCppOwm.cpp
    ../src/net/Curly.cpp
    ../src/tasks/Task.cpp
    ../src/tasks/TaskManager.cpp
    ../src/util/Directories.cpp
    ../src/util/Strings.cpp
    api/Types.cpp
    conf/Configuration.cpp
    data/Location.cpp
    data/Weather.cpp
    db/ConnectionInformation.cpp
    tasks/Task.cpp
    tasks/TaskManager.cpp
    util/Directories.cpp
    util/Strings.cpp
    main.cpp)

add_definitions(-std=c++11)
if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -O3 -fexceptions -std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather_information_collector_tests ${weather_information_collector_tests_sources})

# find cURL library
find_package (CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  target_link_libraries (weather_information_collector_tests ${CURL_LIBRARIES})
else ()
  message ( FATAL_ERROR "cURL was not found!" )
endif (CURL_FOUND)

# find jsoncpp library
set(LibJsonCpp_DIR "../cmake/" )
find_package (LibJsonCpp)
if (LIBJSONCPP_FOUND)
  include_directories(${LIBJSONCPP_INCLUDE_DIRS})
  target_link_libraries (weather_information_collector_tests ${LIBJSONCPP_LIBRARIES})
else ()
  message ( FATAL_ERROR "libjsoncpp was not found!" )
endif (LIBJSONCPP_FOUND)

# find Boost library: filesystem (+ system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(weather_information_collector_tests ${Boost_LIBRARIES})
else ()
  message ( FATAL_ERROR "Boost library was not found!" )
endif (Boost_FOUND)

# test run
add_test(NAME test-suite
         COMMAND $<TARGET_FILE:weather_information_collector_tests>)


# Recurse into subdirectory for Apixu tests.
add_subdirectory (api/apixu)

# Recurse into subdirectory for DarkSky tests.
add_subdirectory (api/darksky)

# Recurse into subdirectory for OpenWeatherMap tests.
add_subdirectory (api/owm)

# Recurse into subdirectory for executable tests.
add_subdirectory (collector)

# Recurse into subdirectory for configtest executable tests.
add_subdirectory (configtest)

# Recurse into subdirectory for creator executable tests.
add_subdirectory (creator)

# Recurse into subdirectory for update executable tests.
add_subdirectory (update)

# Recurse into subdirectory for TaskManager tests.
add_subdirectory (tasks/manager)
