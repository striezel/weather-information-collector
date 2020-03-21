/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_API_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_API_HPP

#include <mysql++/mysql++.h>
#include "../../api/Types.hpp"
#include "../ConnectionInformation.hpp"

namespace wic
{

namespace db
{

class API
{
  public:
    /** \brief Gets the ID of an API entry in the database.
     *
     * \param ci   database connection information
     * \param type API type for which the ID shall be determined.
     * \return Returns the ID in case of success.
     *         Returns -1, if an error occurred.
     */
    static int getId(const ConnectionInformation& ci, const ApiType type);


    /** \brief Gets the ID of an API entry in the database.
     *
     * \param conn   open database connection
     * \param type API type for which the ID shall be determined.
     * \return Returns the ID in case of success.
     *         Returns -1, if an error occurred.
     */
    static int getId(mysqlpp::Connection& conn, const ApiType type);
}; // class

} // namespace

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_API_HPP
