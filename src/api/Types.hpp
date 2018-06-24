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

#ifndef WEATHER_INFORMATION_COLLECTOR_TYPES_HPP
#define WEATHER_INFORMATION_COLLECTOR_TYPES_HPP

#include <string>

namespace wic
{

/** \brief enumeration type for supported APIs
 */
enum class ApiType { none, Apixu, OpenWeatherMap, DarkSky };


/** \brief converts a name / string to a supported API type
 *
 * \param apiName  name of the API
 * \return Returns the enumeration value for the API type.
 *         Returns ApiType::none, if no match could be found.
 */
ApiType toApiType(const std::string& apiName);


/** \brief gets the name of the API
 *
 * \param type  the API type
 * \return Returns a string that identifies the API type.
 */
std::string toString(const ApiType type);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_TYPES_HPP
