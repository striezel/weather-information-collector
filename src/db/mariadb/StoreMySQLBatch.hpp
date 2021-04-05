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

#ifndef WEATHER_INFORMATION_COLLECTOR_STOREMYSQLBATCH_HPP
#define WEATHER_INFORMATION_COLLECTOR_STOREMYSQLBATCH_HPP

#include <string>
#include "../../data/Weather.hpp"
#include "Connection.hpp"

namespace wic
{

/** \brief Class to store data in a MySQL database in batches.  */
class StoreMySQLBatch
{
  public:
    /** \brief Constructor.
     *
     * \param ci  information for connection to the database
     * \param batchSize  number of records per insert statement
     */
    StoreMySQLBatch(const ConnectionInformation& ci, const unsigned int batchSize);


    /** \brief Destructor.
     */
    ~StoreMySQLBatch();


    /** \brief Saves a current weather entry for a given location and API.
     *
     * \param apiId       id of the API that was used to gather the information
     * \param locationId  id of the location for the weather information
     * \param weather     weather information
     * \return Returns true, if the data was saved.
     *         Returns false, if an error occurred.
     */
    bool saveCurrentWeather(const int apiId, const int locationId, const Weather& weather);
  private:
    bool commit();
    unsigned int records;
    unsigned int batchLimit;
    db::mariadb::Connection conn; /**< MySQL connection */
    std::string insertQuery;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_STOREMYSQLBATCH_HPP
