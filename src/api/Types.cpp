/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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

#include "Types.hpp"
#include "../util/Strings.hpp"

namespace wic
{

ApiType toApiType(const std::string& apiName)
{
  auto name = toLowerString(apiName);
  trim(name);
  // Apixu
  if (name == "apixu")
    return ApiType::Apixu;
  // OpenWeatherMap
  if (name == "openweathermap")
    return ApiType::OpenWeatherMap;
  // DarkSky
  if (name == "darksky")
    return ApiType::DarkSky;
  // unknown / none
  return ApiType::none;
}

std::string toString(const ApiType type)
{
  switch (type)
  {
    case ApiType::Apixu:
         return "Apixu";
    case ApiType::OpenWeatherMap:
         return "OpenWeatherMap";
    case ApiType::DarkSky:
         return "DarkSky";
    case ApiType::none:
    default:
         return "none";
  } // switch
}

} // namespace
