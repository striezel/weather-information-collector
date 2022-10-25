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

#ifndef WEATHER_INFORMATION_COLLECTOR_LIMITS_HPP
#define WEATHER_INFORMATION_COLLECTOR_LIMITS_HPP

#include "Limit.hpp"
#include "Plans.hpp"
#include "Types.hpp"

namespace wic
{

/** \brief structure for request limits of an API */
struct Limits
{
  // delete constructor
  Limits() = delete;


  /** \brief contains the limit for unknown API - always zero
   */
  static const Limit none;


  /** \brief Returns a limit for a particular API.
   *
   * \param api  the corresponding API
   * \param planOwm the pricing plan for OpenWeatherMap
   * \param planWb the pricing plan for Weatherbit
   * \param planWs the pricing plan for Weatherstack
   * \return Returns the request limit for the specified API.
   */
  static Limit forApi(const ApiType api, const PlanOwm planOwm, const PlanWeatherbit planWb, const PlanWeatherstack planWs);
}; // struct

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_LIMITS_HPP
