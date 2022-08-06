/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2022  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_STRINGS_HPP
#define WEATHER_INFORMATION_COLLECTOR_STRINGS_HPP

#include <string>

namespace wic
{

/** \brief Returns the lower case version of the given string.
 *
 * \param str  the string
 * \return Returns the string where all upper case characters are turned into
 *         lower case characters.
 */
std::string toLowerString(std::string str);


/** \brief Removes all leading and trailing spaces and (horizontal) tabulators
 *  from the given string.
 *
 * \param str1  the string that shall be trimmed
 */
void trim(std::string& str1);


/** \brief Tries to convert the string representation of an integer number into an int.
 *
 * \param str   the string that contains the number
 * \param value the int that will be used to store the result
 * \return Returns true on success, false on failure.
 * \remarks If false is returned, the value of parameter value is undefined.
 */
bool stringToInt(const std::string& str, int& value);


/** \brief Converts a floating point value into its string representation.
 *
 * \param f  the floating point value
 * \return Returns string representation of the floating point value.
 */
std::string floatToString(const float f);


/** \brief Tries to convert the string representation of a floating point value into a float.
 *
 * \param str    the string that contains the number
 * \param value  the float that will be used to store the result
 * \return Returns true on success, false on failure.
 * \remarks If false is returned, the value of parameter value is undefined.
 */
bool stringToFloat(const std::string& str, float& value);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_STRINGS_HPP
