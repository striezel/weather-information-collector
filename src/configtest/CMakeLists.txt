# Might work with earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

project(weather-information-collector-configtest)

set(weather-information-collector-configtest_sources
    ../api/Limits.cpp
    ../api/Types.cpp
    ../conf/Configuration.cpp
    ../data/Location.cpp
    ../db/ConnectionInformation.cpp
    ../tasks/Task.cpp
    ../tasks/TaskManager.cpp
    ../util/GitInfos.cpp
    ../util/Strings.cpp
    main.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -O3 -fexceptions)
endif (CMAKE_COMPILER_IS_GNUCC)
add_definitions(-std=c++11)

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -s" )

add_executable(weather-information-collector-configtest ${weather-information-collector-configtest_sources})

# find Boost library: filesystem (+ system as dependency)
find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem system)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(weather-information-collector-configtest ${Boost_LIBRARIES})
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
