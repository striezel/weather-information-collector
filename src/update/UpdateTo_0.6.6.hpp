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

#ifndef WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_6_6_HPP
#define WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_6_6_HPP

#include "../db/ConnectionInformation.hpp"

namespace wic
{

/** Manages update of databases before version 0.6.6 of
    weather-information-collector to version 0.6.6.

    This class basically does two things to the database:
    * It changes the type of column `json` in the table `weatherdata` from TEXT
      (can hold up to 65535 characters) to MEDIUMTEXT (can hold ca. 16 million
      characters), because some JSON answers from DarkSky API are longer than
      the old limit of 65535 characters.
    * Existing data sets in the table `weatherdata` where the content of the
      column `json` has a length of 65535 characters will be set to NULL,
      because the existing JSON content for that columns is truncated and thus
      not valid JSON anymore.
*/
class UpdateTo066
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
     * \return Returns 1, if the update was successful.
     *         Returns 0, if no update was necessary.
     *         Returns -1, if an error occurred.
     */
    static int updateStructure(const ConnectionInformation& ci) noexcept;


    /** \brief Performs the data changes of the update.
     *
     * \param ci   database connection information
     * \return Returns true, if the update was successful.
     *         Returns false otherwise.
     */
    static bool updateData(const ConnectionInformation& ci) noexcept;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_UPDATE_TO_0_6_6_HPP
