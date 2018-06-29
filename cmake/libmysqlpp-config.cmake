# - Try to find libmysqlpp / libmysql++
#
# Once done this will define
#  LIBMYSQLPP_FOUND - System has libmysqlpp
#  MYSQLPP_INCLUDE_DIR - The libmysqlpp include directories
#  MYSQLPP_LIBRARIES - The libraries needed to use libmysqlpp

find_path(MYSQLPP_INCLUDE_PATH mysql++.h /usr/include/mysql++/)

find_path(MYSQL_INCLUDE_PATH mysql.h /usr/include/mysql/)
if(MYSQL_INCLUDE_PATH AND MYSQLPP_INCLUDE_PATH)
  set(MYSQLPP_INCLUDE_DIR
    ${MYSQLPP_INCLUDE_PATH}
    ${MYSQL_INCLUDE_PATH}
  )
endif(MYSQL_INCLUDE_PATH AND MYSQLPP_INCLUDE_PATH)

find_library(MYSQLPP_LIBRARIES mysqlpp)

if (MYSQLPP_INCLUDE_DIR AND MYSQLPP_LIBRARIES)
   set(MYSQLPP_FOUND TRUE)
else (MYSQLPP_INCLUDE_DIR AND MYSQLPP_LIBRARIES)
   set(MYSQLPP_FOUND FALSE)
endif (MYSQLPP_INCLUDE_DIR AND MYSQLPP_LIBRARIES)

if (MYSQLPP_FIND_REQUIRED)
  if (MYSQLPP_FOUND)
  else (MYSQLPP_FOUND)
    message(FATAL_ERROR "Could not find mysqlpp!")
  endif(MYSQLPP_FOUND)
endif(MYSQLPP_FIND_REQUIRED)

mark_as_advanced(MYSQLPP_LIBRARIES MYSQLPP_INCLUDE_DIR)
