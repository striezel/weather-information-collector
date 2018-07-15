/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_SOURCEMYSQL_HPP
#define WEATHER_INFORMATION_COLLECTOR_SOURCEMYSQL_HPP

#include <chrono>
#include <map>
#include <vector>
#include "../api/Types.hpp"
#include "../data/Location.hpp"
#include "../data/Weather.hpp"
#include "../db/ConnectionInformation.hpp"

namespace wic
{

/** \brief Retrieves data from a MySQL database. */
class SourceMySQL
{
  public:
    /** \brief Constructor
     *
     * \param ci  information for connection to the database
     */
    SourceMySQL(const ConnectionInformation& ci);


    /** \brief Destructor.
     */
    virtual ~SourceMySQL();


    /** \brief Gets the current weather data for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param weather   container to store the retrieved information
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    virtual bool getCurrentWeather(const ApiType type, const Location& location, std::vector<Weather>& weather);


    /** \brief Lists all APIs that are present in the database.
     *
     * \param apis map to store the available apis; key is API, value is its ID
     *             in the database
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    virtual bool listApis(std::map<ApiType, int>& apis);


    /** \brief Lists all named locations together with the APIs from which data
     * for that locations are present in the database.
     *
     * \param locations used to return location-api pairs
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    virtual bool listLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations);

    /** \brief Gets the ID of a location from the database. If no such location
     * exists, it will be created.
     *
     * \param location  the location to look for
     * \return Returns the ID of the location in case of success.
     * Returns -1, if an error occurred.
     */
    virtual int getLocationId(const Location& location);


    /** \brief Checks whether there is a database entry for the given api,
     *         location, data time and request time.
     *
     * \param apiId  id of the api
     * \param locationId id of the location
     * \param dt         time of the weather data ("data time")
     * \param rt         time of the data request ("request time")
     * \return Returns true, if the entry is present.
     *         Returns false, if there is no such entry or there was an error.
     */
    virtual bool hasEntry(int apiId, int locationId,
                          const std::chrono::time_point<std::chrono::system_clock>& dt,
                          const std::chrono::time_point<std::chrono::system_clock>& rt);
  private:
    ConnectionInformation connInfo; /**< MySQL connection information */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SOURCEMYSQL_HPP
