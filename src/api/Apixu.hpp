/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_APIXU_HPP
#define WEATHER_INFORMATION_COLLECTOR_APIXU_HPP

#include <string>
#include "API.hpp"

namespace wic
{

class Apixu: public API
{
  public:
    /** \brief constructor
     *
     * \param key  the API key for requests
     */
    Apixu(const std::string& key = "");


    /** \brief set the APi key for API requests
     *
     * \param key  the API key
     */
    virtual void setApiKey(const std::string& key);


    /** \brief checks whether the given location can be used for a request
     *
     * \param location  the location information
     * \return Returns true, if the location can be uses for a request.
     *         Returns false otherwise.
     */
    virtual bool validLocation(const Location& location) const;


    /** \brief retrieves the current weather for a given location
     *
     * \param location  the location for which the weather is requested
     * \param weather   variable where result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool currentWeather(const Location& location, Weather& weather);


    /** \brief parses the current weather information from JSON into the Weather object
     *
     * \param json     string containing the JSON
     * \param weather  variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool parseCurrentWeather(const std::string& json, Weather& weather) const;
  private:
    std::string m_apiKey; /**< the API key for requests */


    /** \brief turns info of a location to a request string
     *
     * \param location  the location information
     * \return Returns part of URL that can be used for a request.
     *         Returns empty string, if an error occurred.
     */
    std::string toRequestString(const Location& location) const;
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_APIXU_HPP