/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_VERSION_HPP
#define WEATHER_INFORMATION_COLLECTOR_VERSION_HPP

#include <string>
#include <string_view>

namespace wic
{

/** \brief version information */
const std::string version = "version 0.9.22, 2022-08-18";


/** \brief Prints version information to the standard output.
 *
 * \param name     name of the program to show
 */
void showVersion(const std::string_view name);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_VERSION_HPP
