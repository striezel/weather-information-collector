/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2021, 2022  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_OPENWEATHERMAP_HPP
#define WEATHER_INFORMATION_COLLECTOR_OPENWEATHERMAP_HPP

#include <string>
#ifdef wic_owm_find_location
#include <utility>
#include <vector>
#include "../data/Location.hpp"
#endif // wic_owm_find_location
#include "API.hpp"

namespace wic
{

/** \brief Handles API requests for OpenWeatherMap.org.
 */
class OpenWeatherMap: public API
{
  public:
    /** \brief Constructor.
     *
     * \param key  the API key for requests
     */
    explicit OpenWeatherMap(const std::string& key = "");


    /** \brief Sets the API key for API requests.
     *
     * \param key  the API key
     */
    void setApiKey(const std::string& key) override;


    /** \brief Indicates whether requests to the API need an API key.
     *
     * \return Returns true, because OpenWeatherMap requests need a key.
     */
    bool needsApiKey() final;


    /** \brief Checks whether the given location can be used for a request.
     *
     * \param location  the location information
     * \return Returns true, if the location can be used for a request.
     *         Returns false otherwise.
     */
    bool validLocation(const Location& location) const override;


    /** \brief Checks whether the given data type is supported by the API.
     *
     * \param data  the data type to check
     * \return Returns true, if the data type is supported.
     *         Returns false otherwise.
     */
    bool supportsDataType(const DataType data) const override;


    #ifndef wic_no_network_requests
    /** \brief Turns info of a location to a request string.
     *
     * \param location  the location information
     * \return Returns part of URL that can be used for a request.
     *         Returns empty string, if an error occurred.
     */
    static std::string toRequestString(const Location& location);


    /** \brief Retrieves the current weather for a given location.
     *
     * \param location  the location for which the weather is requested
     * \param weather   variable where result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    bool currentWeather(const Location& location, Weather& weather) override;


    /** \brief Retrieves the weather forecast for a given location.
     *
     * \param location  the location for which the forecast is requested
     * \param forecast  variable where the result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    bool forecastWeather(const Location& location, Forecast& forecast) override;


    /** \brief Retrieves the current weather and the forecast for a given location.
     *
     * \param location  the location for which the forecast is requested
     * \param weather  variable where current weather result of the request will be stored
     * \param forecast  variable where the forecast result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    bool currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast) override;
    #endif // wic_no_network_requests


    #ifndef wic_no_json_parsing
    /** \brief Parses the current weather information from JSON into the Weather object.
     *
     * \param json     string containing the JSON
     * \param weather  variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    bool parseCurrentWeather(const std::string& json, Weather& weather) const override;


    /** \brief Parses the weather forecast information from JSON into Weather objects.
     *
     * \param json     string containing the JSON
     * \param forecast variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    bool parseForecast(const std::string& json, Forecast& forecast) const override;
    #endif // wic_no_json_parsing


    #ifdef wic_owm_find_location
    /** \brief Finds matching locations by name.
     *
     * \param name  the name of the location to find
     * \param locations  variable where matching locations will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    bool findLocation(const std::string& name, std::vector<std::pair<Location, Weather> >& locations) const;
    #endif // wic_owm_find_location
  private:
    std::string m_apiKey; /**< the API key for requests */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_OPENWEATHERMAP_HPP
