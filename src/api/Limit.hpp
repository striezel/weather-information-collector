/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_LIMIT_HPP
#define WEATHER_INFORMATION_COLLECTOR_LIMIT_HPP

#include <chrono>
#include <cstdint>

namespace wic
{

/** \brief structure for request limits of an API */
struct Limit
{
  /** \brief constructor
   *
   * \param _requests  number of allowed requests
   * \param _timespan  time span for that number of requests
   */
  Limit(const uint_least32_t _requests, const std::chrono::seconds& _timespan);


  uint_least32_t requests; /**< number of allowed requests */
  std::chrono::seconds timespan; /**< corresponding time frame */
}; // struct

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_LIMIT_HPP
