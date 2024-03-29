/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2021, 2022  Dirk Stolle

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
  // Weatherbit
  if (name == "weatherbit")
    return ApiType::Weatherbit;
  if (name == "weatherstack")
    return ApiType::Weatherstack;
  // Open-Meteo
  if (name == "openmeteo")
    return ApiType::OpenMeteo;
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
    case ApiType::Weatherbit:
         return "Weatherbit";
    case ApiType::Weatherstack:
         return "Weatherstack";
    case ApiType::OpenMeteo:
         return "OpenMeteo";
    default: // i. e. ApiType::none
         return "none";
  } // switch
}

bool needsApiKey(const ApiType type)
{
  // All APIs except Open-Meteo need a key.
  return type != ApiType::OpenMeteo;
}

DataType toDataType(const std::string& dataName)
{
  auto name = toLowerString(dataName);
  trim(name);
  // current
  if (name == "current")
    return DataType::Current;
  // forecast
  if (name == "forecast")
    return DataType::Forecast;
  // current and forecast
  if ((name == "currentandforecast") || (name == "current+forecast")
       || (name == "forecast+current"))
    return DataType::CurrentAndForecast;
  // unknown / none
  return DataType::none;
}

std::string toString(const DataType type)
{
  switch (type)
  {
    case DataType::Current:
         return "current";
    case DataType::Forecast:
         return "forecast";
    case DataType::CurrentAndForecast:
         return "current+forecast";
    default: // i. e. DataType::none
         return "none";
  } // switch
}

} // namespace
