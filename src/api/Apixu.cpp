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

#include "Apixu.hpp"
#include <iostream>
#ifndef wic_no_json_parsing
#include "../json/JsonCppApixu.hpp"
#include "../net/Curly.hpp"
#endif // wic_no_json_parsing
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
  // All three data types are supported.
  switch (data)
  {
    case DataType::Current:
    case DataType::Forecast:
    case DataType::CurrentAndForecast:
         return true;
    case DataType::none:
    default:
         return false;
  }
}

std::string Apixu::toRequestString(const Location& location) const
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

#ifndef wic_no_json_parsing
bool Apixu::parseCurrentWeather(const std::string& json, Weather& weather) const
{
  return JsonCppApixu::parseCurrentWeather(json, weather);
}

bool Apixu::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.apixu.com/v1/current.json?key="
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
      std::cerr << "Error in Apixu::currentWeather(): Unexpected HTTP status code "
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

bool Apixu::parseForecast(const std::string& json, Forecast& forecast) const
{
  return JsonCppApixu::parseForecast(json, forecast);
}

bool Apixu::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;

  const std::string url = "https://api.apixu.com/v1/forecast.json?days=7&key="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    forecast.setRequestTime(std::chrono::system_clock::now());
    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in Apixu::forecastWeather(): Unexpected HTTP status code "
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

  // Parsing is done in separate method.
  return parseForecast(response, forecast);
}

bool Apixu::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  weather = Weather();
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;

  const std::string url = "https://api.apixu.com/v1/forecast.json?days=7&key="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    weather.setRequestTime(std::chrono::system_clock::now());
    forecast.setRequestTime(weather.requestTime());
    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in Apixu::currentAndForecastWeather(): Unexpected HTTP status code "
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

  // Parsing is done in separate methods.
  if (!parseCurrentWeather(response, weather))
    return false;
  return parseForecast(response, forecast);
}
#endif // wic_no_json_parsing

} // namespace
