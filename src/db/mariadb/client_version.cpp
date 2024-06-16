/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2021, 2022, 2024  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#include "client_version.hpp"
#if defined(__has_include)
  #if __has_include(<mariadb/mysql.h>)
    #include <mariadb/mysql.h>
  #else
    #include <mysql/mysql.h>
  #endif
#else
  // If there is no __has_include, just go with mariadb.
  #include <mariadb/mysql.h>
#endif
#include <iostream>

namespace wic
{

void showMariaDbClientVersion()
{
  const char * client_info = mysql_get_client_info();
  if (client_info != nullptr && client_info[0] != '\0')
  {
    std::cout << "MariaDB client version: " << client_info << std::endl;
  }
  else
  {
    std::cout << "MariaDB client library: unknown version" << std::endl;
  }
}

} // namespace
