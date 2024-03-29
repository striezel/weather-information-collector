/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_UTILITIES_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_UTILITIES_HPP

#include <cstdint>
#include "../../data/Location.hpp"
#include "Connection.hpp"

namespace wic
{

/** \brief Gets the ID of a location from the database. If no such location
 * exists, it will be created.
 *
 * \param conn      MariaDB connection
 * \param location  the location to look for
 * \return Returns the ID of the location in case of success.
 * Returns -1, if an error occurred.
 */
int_least32_t getLocationId(db::mariadb::Connection& conn, const Location& location);


/** \brief Gets a location by ID from the database.
 *
 * \param conn MariaDB connection
 * \param locationId ID of the location
 * \return Returns the location with the given ID, if it was found.
 *         Returns an empty location otherwise.
 */
Location getLocation(db::mariadb::Connection& conn, const int_least32_t locationId);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_UTILITIES_HPP
