/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_SIMDJSONOWM_HPP
#define WEATHER_INFORMATION_COLLECTOR_SIMDJSONOWM_HPP

#include "../../third-party/simdjson/simdjson.h"
#include "../data/Forecast.hpp"
#include "../data/Weather.hpp"
#ifdef wic_owm_find_location
#include "../data/Location.hpp"
#endif // wic_owm_find_location

namespace wic
{

/** \brief Handles JSON input from the OpenWeatherMap API with the simdjson library.
 */
class SimdJsonOwm
{
  public:
    /** \brief Parses the current weather information from JSON into the Weather object.
     *
     * \param json     string containing the JSON
     * \param weather  variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    static bool parseCurrentWeather(const std::string& json, Weather& weather);


    /** \brief Parses the weather forecast information from JSON into Weather objects.
     *
     * \param json     string containing the JSON
     * \param forecast variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    static bool parseForecast(const std::string& json, Forecast& forecast);

    #ifdef wic_owm_find_location
    /** \brief Parses found locations.
     *
     * \param json     string containing the JSON
     * \param location  variable where parsed locations will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    static bool parseLocations(const std::string& json, std::vector<std::pair<Location, Weather> >& locations);
    #endif // wic_owm_find_location
  private:
    // alias for type that keeps JSON values / objects / arrays, etc.
    using value_type = simdjson::dom::element;


    /** \brief Parses weather data from a single JSON weather item into an
     * instance of Weather class.
     *
     * \param value  the JSON value to parse
     * \param weather the Weather item where the data shall be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    static bool parseSingleWeatherItem(const value_type& value, Weather& weather);
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SIMDJSONOWM_HPP
