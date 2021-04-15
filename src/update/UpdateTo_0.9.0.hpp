/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_9_0_HPP
#define WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_9_0_HPP

#include "../db/ConnectionInformation.hpp"

namespace wic
{

/** Manages update of databases before version 0.9.0 of
    weather-information-collector to version 0.9.0.

    This class basically does three things to the database:
    * It adds the column country_code to the table `location` in the database.
    * It tries to find a matching country code for every entry in the table
      `location` by looking at existing weather data from OpenWeatherMap
      (because those contain a country code), and sets the country code, if one
      was found.
    * It adds a new entry for the Weatherbit API to the table `api`.
*/
class UpdateTo090
{
  public:
    /** \brief Performs the update.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool perform(const ConnectionInformation& ci) noexcept;


    /** \brief Performs the database structure changes of the update.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool updateStructure(const ConnectionInformation& ci) noexcept;


    /** \brief Performs the data changes of the update.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool updateData(const ConnectionInformation& ci) noexcept;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_9_0_HPP
