/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2020, 2021  Dirk Stolle

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

#include "Weatherstack.hpp"
#include <iostream>
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonWeatherstack.hpp"
#else
#include "../json/NLohmannJsonWeatherstack.hpp"
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Curly.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

Weatherstack::Weatherstack(const PlanWeatherstack plan, const std::string& key)
: m_apiKey(key),
  m_plan(plan)
{
}

void Weatherstack::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool Weatherstack::validLocation(const Location& location) const
{
  return (location.hasCoordinates() || location.hasName()
       || location.hasPostcode());
}

bool Weatherstack::supportsDataType(const DataType data) const
{
  // At the moment only current weather is supported.
  // Other types (DataType::Forecast, DataType::CurrentAndForecast, and of
  // course DataType::none) are not supported.
  return (data == DataType::Current);
}

#ifndef wic_no_json_parsing
bool Weatherstack::parseCurrentWeather(const std::string& json, Weather& weather) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonWeatherstack::parseCurrentWeather(json, weather);
#else
  return NLohmannJsonWeatherstack::parseCurrentWeather(json, weather);
#endif // __SIZEOF_INT128__
}

bool Weatherstack::parseForecast(const std::string& json, Forecast& forecast) const
{
  // Not implemented.
  return false;
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string Weatherstack::toRequestString(const Location& location) const
{
  if (location.hasCoordinates())
    return std::string("query=") + floatToString(location.latitude())
         + std::string(",") + floatToString(location.longitude());
  if (location.hasName())
    return std::string("query=") + location.name();
  if (location.hasPostcode())
    return std::string("query=") + location.postcode();
  // no required data set
  return std::string();
}

bool Weatherstack::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  // Free plan only supports HTTP, higher plans allow HTTPS.
  const std::string url = ((m_plan == PlanWeatherstack::Free)
          ? "http://api.weatherstack.com/current?access_key="
          : "https://api.weatherstack.com/current?access_key=")
                  + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    weather.setRequestTime(std::chrono::system_clock::now());
    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in Weatherstack::currentWeather(): Unexpected HTTP status code "
                << curly.getResponseCode() << "!" << std::endl;
      const auto & rh = curly.responseHeaders();
      std::cerr << "HTTP response headers (" << rh.size() << "):" << std::endl;
      for (const auto & s : rh)
      {
        std::cerr << "    " << s << std::endl;
      }
      return false;
    }
  } // scope of curly

  return parseCurrentWeather(response, weather);
}

bool Weatherstack::forecastWeather(const Location& location, Forecast& forecast)
{
  // Not implemented.
  forecast = Forecast();
  return false;
}

bool Weatherstack::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  // Not implemented.
  weather = Weather();
  forecast = Forecast();
  return false;
}
#endif // wic_no_network_requests

} // namespace
