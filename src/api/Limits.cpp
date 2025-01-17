/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2025  Dirk Stolle

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

/* There's no limit here for "none" API, but set it to zero. */
const Limit Limits::none = Limit(0, std::chrono::hours(1));

Limit Limits::forApi(const ApiType api, const PlanOwm planOwm, const PlanWeatherbit planWb, const PlanWeatherstack planWs)
{
  switch(api)
  {
    case ApiType::Apixu:
         /* Limit for Apixu was 10000 calls per month on the free plan,
            which is ca. 13.4 requests per hour. */
         return Limit(10000, std::chrono::hours(24 * 31));
    case ApiType::OpenWeatherMap:
         switch (planOwm)
         {
             case PlanOwm::Free:
                  /* Limit for OpenWeatherMap is 60 calls per minute on the free plan. */
                  return Limit(60, std::chrono::minutes(1));
             case PlanOwm::Startup:
                  /* Limit for OpenWeatherMap is 600 calls per minute on the Startup plan. */
                  return Limit(600, std::chrono::minutes(1));
             case PlanOwm::Developer:
                  /* Limit for OpenWeatherMap is 3000 calls per minute on the Developer plan. */
                  return Limit(3000, std::chrono::minutes(1));
             case PlanOwm::Professional:
                  /* Limit for OpenWeatherMap is 30000 calls per minute on the Professional plan. */
                  return Limit(30000, std::chrono::minutes(1));
             case PlanOwm::Enterprise:
                  /* Limit for OpenWeatherMap is 200000 calls per minute on the Professional plan. */
                  return Limit(200000, std::chrono::minutes(1));
             default: // i. e. PlanOwm ::none
                 return Limits::none;
         }
    case ApiType::DarkSky:
         /* Limit for DarkSky is 1000 calls per day on the free plan,
            which is ca. 41.667 requests per hour. */
         return Limit(1000, std::chrono::hours(24));
    case ApiType::Weatherbit:
         switch (planWb)
         {
             case PlanWeatherbit::Free:
                  /* Limit for Weatherbit is 50 calls per day on the free plan,
                     which is ca. 2.0833 requests per hour. */
                  return Limit(50, std::chrono::hours(24));
             case PlanWeatherbit::Hobbyist:
                  /* Limit for Weatherbit is 350 calls per day on the Hobbyist plan,
                     which is ca. 14.5833 requests per hour. */
                  return Limit(350, std::chrono::hours(24));
             case PlanWeatherbit::Starter:
                  /* Limit for Weatherbit is 50000 calls per day on the Starter plan,
                     which is ca. 2083.33 requests per hour. */
                  return Limit(50000, std::chrono::hours(24));
             case PlanWeatherbit::Developer:
                  /* Limit for Weatherbit is 500000 calls per day on the Starter plan,
                     which is ca. 20833.33 requests per hour. */
                 return Limit(500000, std::chrono::hours(24));
             case PlanWeatherbit::Advanced:
                  /* Limit for Weatherbit is 5000000 calls per day on the Starter plan,
                     which is ca. 208333.33 requests per hour. */
                 return Limit(5000000, std::chrono::hours(24));
             default: // i. e. PlanWeatherbit::none
                  return Limits::none;
         }
    case ApiType::Weatherstack:
         switch (planWs)
         {
             case PlanWeatherstack::Free:
                  /* Limit for Weatherstack is 100 calls per month on the free plan,
                     which is ca. 0.134 requests per hour or ca. 3 requests per day. */
                  return Limit(100, std::chrono::hours(24 * 31));
             case PlanWeatherstack::Standard:
                  /* Limit for Weatherstack is 50000 calls per month on the Standard plan,
                     which is ca. 67.2 requests per hour. */
                  return Limit(50000, std::chrono::hours(24 * 31));
             case PlanWeatherstack::Professional:
                  /* Limit for Weatherstack is 300000 calls per month on the Professional plan,
                     which is ca. 403.23 requests per hour. */
                 return Limit(300000, std::chrono::hours(24 * 31));
             case PlanWeatherstack::Business:
                 /* Limit for Weatherstack is one million calls per month on the Business plan,
                    which is ca. 1344.09 requests per hour. */
                 return Limit(1000000, std::chrono::hours(24 * 31));
             default: // i. e. PlanWeatherstack::none
                  return Limits::none;
         }
    case ApiType::OpenMeteo:
         /* Open-Meteo does not seem to have a hard limit, but the documentation
            asks to not use more than 10000 calls per day, so we use that. */
         return Limit(10000, std::chrono::hours(24));
    default: // i. e. ApiType::none
         // no limit
         return Limits::none;
  }
}

} // namespace
