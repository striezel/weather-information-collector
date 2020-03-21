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

#ifndef WEATHER_INFORMATION_COLLECTOR_STORE_HPP
#define WEATHER_INFORMATION_COLLECTOR_STORE_HPP

#include "../api/Types.hpp"
#include "../data/Forecast.hpp"
#include "../data/Location.hpp"
#include "../data/Weather.hpp"

namespace wic
{

/** \brief Interface for weather data storage. */
class Store
{
  public:
    /** \brief Saves a current weather entry for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param weather   weather information
     * \return Returns true, if the data was saved.
     *         Returns false, if an error occurred.
     */
    virtual bool saveCurrentWeather(const ApiType type, const Location& location, const Weather& weather) = 0;


    /** \brief Saves weather forecast data for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param forecast  weather forecast information
     * \return Returns true, if the data was saved.
     *         Returns false, if an error occurred.
     */
    virtual bool saveForecast(const ApiType type, const Location& location, const Forecast& forecast) = 0;


    /** \brief Virtual destructor, because of derived classes.
     */
    virtual ~Store() { }
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_STORE_HPP
