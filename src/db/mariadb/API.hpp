/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2020, 2021, 2022  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_API_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_API_HPP

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
#include "../../api/Types.hpp"
#include "../ConnectionInformation.hpp"
#include "Connection.hpp"

namespace wic::db
{

class API
{
  public:
    /** \brief Gets the ID of an API entry in the database.
     *
     * \param ci   database connection information
     * \param type API type for which the ID shall be determined.
     * \return Returns the ID in case of success.
     *         Returns zero, if no matching entry was found.
     *         Returns -1, if an error occurred.
     */
    static int getId(const ConnectionInformation& ci, const ApiType type);


    /** \brief Gets the ID of an API entry in the database.
     *
     * \param conn   open database connection
     * \param type API type for which the ID shall be determined.
     * \return Returns the ID in case of success.
     *         Returns zero, if no matching entry was found.
     *         Returns -1, if an error occurred.
     */
    static int getId(const mariadb::Connection& conn, const ApiType type);
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_API_HPP
