/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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
#include <jsoncpp/json/reader.h>
#include "API.hpp"

namespace wic
{

class OpenWeatherMap: public API
{
  public:
    /** \brief Constructor.
     *
     * \param key  the API key for requests
     */
    OpenWeatherMap(const std::string& key = "");


    /** \brief Sets the API key for API requests.
     *
     * \param key  the API key
     */
    virtual void setApiKey(const std::string& key);


    /** \brief Checks whether the given location can be used for a request.
     *
     * \param location  the location information
     * \return Returns true, if the location can be used for a request.
     *         Returns false otherwise.
     */
    virtual bool validLocation(const Location& location) const;


    /** \brief Checks whether the given data type is supported by the API.
     *
     * \param data  the data type to check
     * \return Returns true, if the data type is supported.
     *         Returns false otherwise.
     */
    virtual bool supportsDataType(const DataType data) const;


    /** \brief Retrieves the current weather for a given location.
     *
     * \param location  the location for which the weather is requested
     * \param weather   variable where result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool currentWeather(const Location& location, Weather& weather);


    /** \brief Parses the current weather information from JSON into the Weather object.
     *
     * \param json     string containing the JSON
     * \param weather  variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool parseCurrentWeather(const std::string& json, Weather& weather) const;


    /** \brief Retrieves the weather forecast for a given location.
     *
     * \param location  the location for which the forecast is requested
     * \param forecast  variable where the result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool forecastWeather(const Location& location, Forecast& forecast);


    /** \brief Parses the weather forecast information from JSON into Weather objects.
     *
     * \param json     string containing the JSON
     * \param forecast variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool parseForecast(const std::string& json, Forecast& forecast) const;
  private:
    std::string m_apiKey; /**< the API key for requests */


    /** \brief Parses weather data from a single JSON weather item into an
     * instance of Weather class.
     *
     * \param value  the JSON value to parse
     * \param weather the Weather item where the data shall be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    bool parseSingleWeatherItem(const Json::Value& value, Weather& weather) const;


    /** \brief Turns info of a location to a request string.
     *
     * \param location  the location information
     * \return Returns part of URL that can be used for a request.
     *         Returns empty string, if an error occurred.
     */
    std::string toRequestString(const Location& location) const;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_OPENWEATHERMAP_HPP
