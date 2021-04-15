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

#ifndef WEATHER_INFORMATION_COLLECTOR_UPDATETO_0_8_3_HPP
#define WEATHER_INFORMATION_COLLECTOR_UPDATETO_0_8_3_HPP

#include "../db/ConnectionInformation.hpp"

namespace wic
{

/** Manages update of databases before version 0.8.3 of
    weather-information-collector to version 0.8.3.

    This class basically does two things to the database:
    * It adds the column `snow` to the tables `weatherdata` and `forecastdata`,
      if it does not exist yet.
    * Existing data sets in the table `weatherdata` that do not have any snow
      data yet but still have the original JSON response will get their snow
      data set by re-parsing the JSON data and setting it to the value from the
      JSON content.
*/
class UpdateTo083
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
  private:
    /** \brief Updates the snow information in the table weatherdata.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool updateWeatherData(const ConnectionInformation& ci) noexcept;

    /** \brief Updates the snow information in the table forecastdata.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool updateForecastData(const ConnectionInformation& ci) noexcept;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_UPDATETO_0_8_3_HPP
