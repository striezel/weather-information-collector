/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019, 2020  Dirk Stolle

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

#include "Factory.hpp"
#include <iostream>
#include "Apixu.hpp"
#include "DarkSky.hpp"
#include "OpenWeatherMap.hpp"
#include "Weatherbit.hpp"
#include "Weatherstack.hpp"

namespace wic
{

std::unique_ptr<API> Factory::create(const ApiType api, const PlanWeatherstack planWs, const std::string& key)
{
  std::unique_ptr<API> result = nullptr;
  switch (api)
  {
    case ApiType::OpenWeatherMap:
         result.reset(new OpenWeatherMap(key));
         break;
    case ApiType::Apixu:
         result.reset(new Apixu(key));
         break;
    case ApiType::DarkSky:
         result.reset(new DarkSky(key));
         break;
    case ApiType::Weatherbit:
         result.reset(new Weatherbit(key));
         break;
    case ApiType::Weatherstack:
         result.reset(new Weatherstack(planWs, key));
         break;
    case ApiType::none:
    default:
         std::cerr << "Error: API type " << toString(api) << " is not supported by API factory!" << std::endl;
         return nullptr;
  } // switch
  return result;
}

} // namespace
