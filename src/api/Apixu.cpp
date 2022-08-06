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

#include "Apixu.hpp"
#include <iostream>
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonApixu.hpp"
#else
#include "../json/NLohmannJsonApixu.hpp"
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Request.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

Apixu::Apixu(const std::string& key)
: m_apiKey(key)
{
}

void Apixu::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool Apixu::validLocation(const Location& location) const
{
  return (location.hasCoordinates() || location.hasName()
       || location.hasPostcode());
}

bool Apixu::supportsDataType(const DataType data) const
{
  // All three data types (DataType::Current, DataType::Forecast, and
  // DataType::CurrentAndForecast) are supported.
  return (data != DataType::none);
}

#ifndef wic_no_json_parsing
bool Apixu::parseCurrentWeather(const std::string& json, Weather& weather) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonApixu::parseCurrentWeather(json, weather);
#else
  return NLohmannJsonApixu::parseCurrentWeather(json, weather);
#endif // __SIZEOF_INT128__
}

bool Apixu::parseForecast(const std::string& json, Forecast& forecast) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonApixu::parseForecast(json, forecast);
#else
  return NLohmannJsonApixu::parseForecast(json, forecast);
#endif // __SIZEOF_INT128__
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string Apixu::toRequestString(const Location& location)
{
  if (location.hasCoordinates())
    return std::string("q=") + floatToString(location.latitude())
         + std::string(",") + floatToString(location.longitude());
  if (location.hasName())
    return std::string("q=") + location.name();
  if (location.hasPostcode())
    return std::string("q=") + location.postcode();
  // no required data set
  return std::string();
}

bool Apixu::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.apixu.com/v1/current.json?key="
                        + m_apiKey + "&" + toRequestString(location);
  weather.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "Apixu::currentWeather");
  if (!response.has_value())
    return false;

  return parseCurrentWeather(response.value(), weather);
}

bool Apixu::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;

  const std::string url = "https://api.apixu.com/v1/forecast.json?days=7&key="
                        + m_apiKey + "&" + toRequestString(location);
  forecast.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "Apixu::forecastWeather");
  if (!response.has_value())
    return false;

  // Parsing is done in separate method.
  return parseForecast(response.value(), forecast);
}

bool Apixu::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  weather = Weather();
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;

  const std::string url = "https://api.apixu.com/v1/forecast.json?days=7&key="
                        + m_apiKey + "&" + toRequestString(location);
  weather.setRequestTime(std::chrono::system_clock::now());
  forecast.setRequestTime(weather.requestTime());
  const auto response = Request::get(url, "Apixu::currentAndForecastWeather");
  if (!response.has_value())
    return false;

  // Parsing is done in separate methods.
  if (!parseCurrentWeather(response.value(), weather))
    return false;
  return parseForecast(response.value(), forecast);
}
#endif // wic_no_network_requests

} // namespace
