/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019  Dirk Stolle

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

#include "Limits.hpp"

namespace wic
{

/* Limit for Apixu is 10000 calls per month on the free plan,
   which is ca. 13.4 requests per hour. */
const Limit Limit::apixu = Limit(10000, std::chrono::hours(24 * 31));

/* Limit for OpenWeatherMap is 60 calls per minute on the free plan. */
const Limit Limit::owm = Limit(60, std::chrono::minutes(1));

/* Limit for DarkSky is 1000 calls per day on the free plan,
   which is ca. 41.667 requests per hour. */
const Limit Limit::darksky = Limit(1000, std::chrono::hours(24));

/* Limit for Weatherbit is 1000 calls per day on the free plan,
   which is ca. 41.667 requests per hour. */
const Limit Limit::weatherbit = Limit(1000, std::chrono::hours(24));

/* Limit for Weatherstack is 1000 calls per month on the free plan,
   which is ca. 1.34 requests per hour. */
const Limit Limit::weatherstack = Limit(1000, std::chrono::hours(24 * 31));

/* There's no limit here for "none" API, but set it to zero. */
const Limit Limit::none = Limit(0, std::chrono::hours(1));

Limit::Limit(const uint_least32_t _requests, const std::chrono::seconds& _timespan)
: requests(_requests),
  timespan(_timespan)
{
}

const Limit& Limit::forApi(const ApiType api)
{
  switch(api)
  {
    case ApiType::Apixu:
         return Limit::apixu;
    case ApiType::OpenWeatherMap:
         return Limit::owm;
    case ApiType::DarkSky:
         return Limit::darksky;
    case ApiType::Weatherbit:
         return Limit::weatherbit;
    case ApiType::Weatherstack:
         return Limit::weatherstack;
    case ApiType::none:
         // no limit
         return Limit::none;
  }
  // Fallback, just in case there might be more APIs in the future.
  return Limit::none;
}

} // namespace
