# - Try to find libmysqlclient
#
# Once done this will define
#  MYSQLCLIENT_FOUND - System has libmysqlclient
#  MYSQLCLIENT_INCLUDE_DIR - The libmysqlclient include directories
#  MYSQLCLIENT_LIBRARIES - The libraries needed to use libmysqlclient

find_path(MYSQLCLIENT_INCLUDE_PATH mysql.h /usr/include/mysql/)
if(MYSQLCLIENT_INCLUDE_PATH)
  set(MYSQLCLIENT_INCLUDE_DIR
    ${MYSQLCLIENT_INCLUDE_PATH}
  )
else()
  message ( "Include path for libmysqlclient could not be determined!")
endif(MYSQLCLIENT_INCLUDE_PATH)

find_library(MYSQLCLIENT_LIBRARIES mysqlclient)

if (MYSQLCLIENT_LIBRARIES)
else (MYSQLCLIENT_LIBRARIES)
   message( "Library of libmysqlclient was not found!")
endif (MYSQLCLIENT_LIBRARIES)


if (MYSQLCLIENT_INCLUDE_DIR AND MYSQLCLIENT_LIBRARIES)
   set(MYSQLCLIENT_FOUND TRUE)
else (MYSQLCLIENT_INCLUDE_DIR AND MYSQLCLIENT_LIBRARIES)
   set(MYSQLCLIENT_FOUND FALSE)
endif (MYSQLCLIENT_INCLUDE_DIR AND MYSQLCLIENT_LIBRARIES)

if (MYSQLCLIENT_FIND_REQUIRED)
  if (MYSQLCLIENT_FOUND)
  else (MYSQLCLIENT_FOUND)
    message(FATAL_ERROR "Could not find mysqlclient!")
  endif(MYSQLCLIENT_FOUND)
endif(MYSQLCLIENT_FIND_REQUIRED)

mark_as_advanced(MYSQLCLIENT_LIBRARIES MYSQLCLIENT_INCLUDE_DIR)
