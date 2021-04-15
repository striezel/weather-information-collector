/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_UPDATETO_0_8_1_HPP
#define WEATHER_INFORMATION_COLLECTOR_UPDATETO_0_8_1_HPP

#include "../db/ConnectionInformation.hpp"

namespace wic
{

/** Manages incremental update of databases before version 0.8.1 of
    weather-information-collector to version 0.8.1.

    This class basically does one thing to the database:
    * It rounds values in the columns `temperature_C`, `temperature_F` and
      `temperature_K` in the tables `weatherdata` and `forecastdata`, where
      applicable. This is to avoid values like e.g. 6.99996 instead of 7.0 in
      the database.
*/
class UpdateTo081
{
  public:
    /** \brief Performs the update.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool perform(const ConnectionInformation& ci) noexcept;
  private:
    /** \brief Performs the data changes of the update.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool updateData(const ConnectionInformation& ci) noexcept;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_UPDATETO_0_8_1_HPP
