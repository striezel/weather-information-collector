/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_WEATHERSTACKFUNCTIONS_HPP
#define WEATHER_INFORMATION_COLLECTOR_WEATHERSTACKFUNCTIONS_HPP

#include "../data/Weather.hpp"

namespace wic
{

/** \brief Decides whether precipitation is rain or snow and sets it.
 *
 * \param amount the amount of precipitation
 * \param weather  weather data instance
 * \param code     weather code from JSON
 */
void weatherstackPrecipitationDistinction(const float amount, Weather& weather, const int code);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_WEATHERSTACKFUNCTIONS_HPP
