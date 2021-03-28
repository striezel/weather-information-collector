/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019, 2020, 2021  Dirk Stolle

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

std::unique_ptr<API> Factory::create(const ApiType api, const PlanWeatherbit planWb, const PlanWeatherstack planWs, const std::string& key)
{
  switch (api)
  {
    case ApiType::OpenWeatherMap:
         return std::make_unique<OpenWeatherMap>(key);
    case ApiType::Apixu:
         return std::make_unique<Apixu>(key);
    case ApiType::DarkSky:
         return std::make_unique<DarkSky>(key);
    case ApiType::Weatherbit:
         return std::make_unique<Weatherbit>(planWb, key);
    case ApiType::Weatherstack:
         return std::make_unique<Weatherstack>(planWs, key);
    default: // ApiType::none
         std::cerr << "Error: API type " << toString(api) << " is not supported by API factory!" << std::endl;
         return nullptr;
  } // switch
}

} // namespace
