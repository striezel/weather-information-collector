/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_5_5_HPP
#define WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_5_5_HPP

#include "../db/ConnectionInformation.hpp"

namespace wic
{

/** Manages update of databases before version 0.5.5 of
    weather-information-collector to version 0.5.5.

    This class basically does two things to the database:
    * It adds the column `rain` to the table `weatherdata`, if it does not
      exist yet.
    * Existing data sets in the table `weatherdata` that do not have any rain
      data yet but still have the original JSON response will get their rain
      data set by re-parsing the JSON data and setting it to the value from the
      JSON content.
*/
class UpdateTo055
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

#endif // WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_5_5_HPP
