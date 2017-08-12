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

#ifndef WEATHER_INFORMATION_COLLECTOR_API_HPP
#define WEATHER_INFORMATION_COLLECTOR_API_HPP

#include <string>
#include "../data/Location.hpp"
#include "../data/Weather.hpp"

namespace wic
{

class API
{
  public:
    /** \brief set the APi key for API requests
     *
     * \param key  the API key
     */
    virtual void setApiKey(const std::string& key) = 0;


    /** \brief retrieves the current weather for a given location
     *
     * \param location  the location for which the weather is requested
     * \param weather   variable where result of the request will be stored
     * \return Returns true, if the request was successful.
     *         Returns false, if an error occurred.
     */
    virtual bool currentWeather(const Location& location, Weather& weather) = 0;
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_API_HPP
