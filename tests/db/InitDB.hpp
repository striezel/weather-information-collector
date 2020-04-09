/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_TESTS_INITDB_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_TESTS_INITDB_HPP

#include "../../src/db/ConnectionInformation.hpp"

namespace wic
{

/** \brief Utility class for database initialization steps. */
class InitDB
{
  public:
    /** \brief Creates a test database on the MySQL / MariaDB server.
     *
     * \param ci  the database connection information
     * \return Returns true, if creation was successful. Returns false otherwise.
     */
    static bool createDb(const ConnectionInformation& ci);


    /** \brief Creates the table api on the MySQL / MariaDB server.
     *
     * \param ci  the database connection information
     * \return Returns true, if creation was successful. Returns false otherwise.
     */
    static bool createTableApi(const ConnectionInformation& ci);

    /** \brief Fills the table api on the MySQL / MariaDB server with some default values.
     *
     * \param ci  the database connection information
     * \return Returns true, if operation was successful. Returns false otherwise.
     */
    static bool fillTableApi(const ConnectionInformation& ci);


    /** \brief Creates the table forecast on the MySQL / MariaDB server.
     *
     * \param ci  the database connection information
     * \return Returns true, if creation was successful. Returns false otherwise.
     */
    static bool createTableForecast(const ConnectionInformation& ci);


    /** \brief Creates the table forecastdata on the MySQL / MariaDB server.
     *
     * \param ci  the database connection information
     * \return Returns true, if creation was successful. Returns false otherwise.
     */
    static bool createTableForecastData(const ConnectionInformation& ci);


    /** \brief Creates the table location on the MySQL / MariaDB server.
     *
     * \param ci  the database connection information
     * \return Returns true, if creation was successful. Returns false otherwise.
     */
    static bool createTableLocation(const ConnectionInformation& ci);


    /** \brief Creates the table weatherdata on the MySQL / MariaDB server.
     *
     * \param ci  the database connection information
     * \return Returns true, if creation was successful. Returns false otherwise.
     */
    static bool createTableWeatherData(const ConnectionInformation& ci);


    /** \brief Tries to downgrade the database scheme to the one of version 0.8.6.
     *
     * \param ci  the database connection information
     * \return Returns true, if operation was successful. Returns false otherwise.
     */
    static bool downgradeTo_0_8_6(const ConnectionInformation& ci);
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_TESTS_INITDB_HPP
