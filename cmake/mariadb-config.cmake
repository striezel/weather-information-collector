# - Try to find libmariadb
#
# Once done this will define
#  MARIADB_FOUND - System has libmariadb
#  MARIADB_INCLUDE_DIR - The libmariadb include directories
#  MARIADB_LIBRARIES - The libraries needed to use libmariadb

find_package(PkgConfig)
pkg_check_modules(PC_LIBMARIADB libmariadb)

find_path(MARIADB_INCLUDE_PATH mysql.h
          HINTS /usr/include/mariadb/ /mingw64/include/mariadb/ ${PC_LIBMARIADB_INCLUDE_DIRS})
if(MARIADB_INCLUDE_PATH)
  set(MARIADB_INCLUDE_DIR
    ${MARIADB_INCLUDE_PATH}
  )
else()
  message ( "Include path for libmariadb could not be determined!")
endif(MARIADB_INCLUDE_PATH)

find_library(MARIADB_LIBRARIES NAMES mariadb libmariadb
             HINTS ${PC_LIBMARIADB_LIBRARY_DIRS})

if (MARIADB_LIBRARIES)
else (MARIADB_LIBRARIES)
   message( "Library of libmariadb was not found!")
endif (MARIADB_LIBRARIES)


if (MARIADB_INCLUDE_DIR AND MARIADB_LIBRARIES)
   set(MARIADB_FOUND TRUE)
else (MARIADB_INCLUDE_DIR AND MARIADB_LIBRARIES)
   set(MARIADB_FOUND FALSE)
endif (MARIADB_INCLUDE_DIR AND MARIADB_LIBRARIES)

if (MARIADB_FIND_REQUIRED)
  if (NOT MARIADB_FOUND)
    message(FATAL_ERROR "Could not find libmariadb!")
  endif(NOT MARIADB_FOUND)
endif(MARIADB_FIND_REQUIRED)

mark_as_advanced(MARIADB_LIBRARIES MARIADB_INCLUDE_DIR)
