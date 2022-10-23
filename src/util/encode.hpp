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

#ifndef WEATHER_INFORMATION_COLLECTOR_URLENCODE_HPP
#define WEATHER_INFORMATION_COLLECTOR_URLENCODE_HPP

#include <string>

namespace wic
{

/** \brief Returns the URL-encoded version of a given string.
 *
 * \param str  the string to encode
 * \return Returns the string where all non-standard characters are
 *         percent-encoded.
 */
std::string urlEncode(const std::string& str);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_URLENCODE_HPP
