/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2022  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_SIMDJSONOPENMETEO_HPP
#define WEATHER_INFORMATION_COLLECTOR_SIMDJSONOPENMETEO_HPP

#include <optional>
#include "../../third-party/simdjson/simdjson.h"
#include "../data/Forecast.hpp"
#include "../data/Weather.hpp"

namespace wic
{

/** \brief Handles JSON input from the OpenMeteo API with the simdjson library.
 */
class SimdJsonOpenMeteo
{
  public:
    /** \brief Parses the weather forecast information from JSON into Weather objects.
     *
     * \param json     string containing the JSON
     * \param forecast variable where result of the parsing process will be stored
     * \return Returns true, if the parsing was successful.
     *         Returns false, if an error occurred.
     */
    static bool parseForecast(const std::string& json, Forecast& forecast);
  private:
    /** \brief Checks whether the units match the expected values.
     *
     * \param doc   root element of the JSON document
     * \return Returns an empty optional, if units are correct.
     *         Returns an error message otherwise.
     */
    static std::optional<std::string> hourlyUnitCheck(const simdjson::dom::element& doc);

    static bool parseTemperature(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parseHumidity(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parseRain(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parseSnowfall(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parsePressure(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parseCloudCover(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parseWindSpeed(const simdjson::dom::element& hourly, std::vector<Weather>& data);
    static bool parseWindDirection(const simdjson::dom::element& hourly, std::vector<Weather>& data);
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SIMDJSONOPENMETEO_HPP
