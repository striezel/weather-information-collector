/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017  Dirk Stolle

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

/* Limit for Apixu is 5000 calls per month on the free plan,
   which is ca. 6.7 requests per hour. */
const Limit Limit::apixu = Limit(5000, std::chrono::hours(24 * 31));

/* Limit for OpenWeatherMap is 60 calls per minute on the free plan. */
const Limit Limit::own = Limit(60, std::chrono::minutes(1));

Limit::Limit(const uint_least32_t _requests, const std::chrono::seconds& _timespan)
: requests(_requests),
  timespan(_timespan)
{
}

} //namespace
