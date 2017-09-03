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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_STRUCTURE_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_STRUCTURE_HPP

#include "ConnectionInformation.hpp"

namespace wic
{

class Structure
{
  public:
    /** \brief Checks whether a database table exists.
     *
     * \param ci   database connection information
     * \param tableName  name of the table
     * \remarks Database name is always the database name from the connection information.
     * \return Returns true, if the given table exists.
     *         Returns false otherwise.
     */
    static bool tableExists(const ConnectionInformation& ci, const std::string& tableName);


    /** \brief Checks whether a given column in a database table exists.
     *
     * \param ci   database connection information
     * \param tableName  name of the table
     * \param column     name of the table column
     * \remarks Database name is always the database name from the connection information.
     * \return Returns true, if the given column exists.
     *         Returns false otherwise.
     */
    static bool columnExists(const ConnectionInformation& ci, const std::string& tableName, const std::string& column);
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_STRUCTURE_HPP
