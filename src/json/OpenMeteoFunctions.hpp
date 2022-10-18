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

#ifndef WEATHER_INFORMATION_COLLECTOR_OPENMETEOFUNCTIONS_HPP
#define WEATHER_INFORMATION_COLLECTOR_OPENMETEOFUNCTIONS_HPP

#include <chrono>
#include <optional>
#include <string>

namespace wic
{

/** \brief Converts a string containing an ISO 8601 time like '2022-09-23T12:34' to a time point.
 *
 * \param value   string containing the time
 * \return Returns the matching time point in case of success.
 *         Returns an empty optional, if the string could not be converted.
 */
std::optional<std::chrono::time_point<std::chrono::system_clock> > isoStringToTime(const std::string& value);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_OPENMETEOFUNCTIONS_HPP
