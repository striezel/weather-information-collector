/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019, 2021, 2022  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_SOURCEMARIADB_HPP
#define WEATHER_INFORMATION_COLLECTOR_SOURCEMARIADB_HPP

#include <chrono>
#include <map>
#include <vector>
#include "../../api/Types.hpp"
#include "../../data/Forecast.hpp"
#ifndef wic_no_metadata
#include "../../data/ForecastMeta.hpp"
#endif // wic_no_metadata
#include "../../data/Location.hpp"
#include "../../data/Weather.hpp"
#ifndef wic_no_metadata
#include "../../data/WeatherMeta.hpp"
#endif // wic_no_metadata
#include "../ConnectionInformation.hpp"

namespace wic
{

/** \brief Retrieves data from a MariaDB (or compatible) database. */
class SourceMariaDB
{
  public:
    /** \brief Constructor
     *
     * \param ci  information for connection to the database
     */
    explicit SourceMariaDB(const ConnectionInformation& ci);


    /** \brief Gets the current weather data for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param weather   container to store the retrieved information
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool getCurrentWeather(const ApiType type, const Location& location, std::vector<Weather>& weather);


#ifndef wic_no_metadata
    /** \brief Gets the metadata of current weather data for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param weather   container to store the retrieved information
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool getMetaCurrentWeather(const ApiType type, const Location& location, std::vector<WeatherMeta>& weather);
#endif // wic_no_metadata


    /** \brief Gets the weather forecast data for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param forecast   container to store the retrieved information
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool getForecasts(const ApiType type, const Location& location, std::vector<Forecast>& forecast);


#ifndef wic_no_metadata
    /** \brief Gets the metadata of weather forecasts for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param forecast   container to store the retrieved information
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool getMetaForecasts(const ApiType type, const Location& location, std::vector<ForecastMeta>& forecast);
#endif // wic_no_metadata


    /** \brief Lists all APIs that are present in the database.
     *
     * \param apis map to store the available apis; key is API, value is its ID
     *             in the database
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool listApis(std::map<ApiType, int>& apis);


    /** \brief Lists all named locations together with the APIs from which
     * weather data for that locations are present in the database. Does only
     * include actual weather data. Forecast data is not considered.
     *
     * \param locations used to return location-api pairs
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool listWeatherLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations);


    /** \brief Lists all named locations together with the APIs from which
     * forecast data for that locations are present in the database. Does only
     * include forecast data. Actual weather data is not considered.
     *
     * \param locations used to return location-api pairs
     * \return Returns true, if the data was retrieved.
     *         Returns false, if an error occurred.
     */
    bool listForecastLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations);


    /** \brief Gets the ID of a location from the database. If no such location
     * exists, it will be created.
     *
     * \param location  the location to look for
     * \return Returns the ID of the location in case of success.
     * Returns -1, if an error occurred.
     */
    int getLocationId(const Location& location);
  private:
    ConnectionInformation connInfo; /**< MariaDB connection information */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SOURCEMARIADB_HPP
