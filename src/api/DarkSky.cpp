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

#include "DarkSky.hpp"
#include <cmath>
#include <iostream>
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonDarkSky.hpp"
#else
#include "../json/NLohmannJsonDarkSky.hpp"
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Curly.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

DarkSky::DarkSky(const std::string& key)
: m_apiKey(key)
{
}

void DarkSky::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool DarkSky::validLocation(const Location& location) const
{
  // Only latitude and longitude are required for a request.
  return location.hasCoordinates();
}

bool DarkSky::supportsDataType(const DataType data) const
{
  // All three data types (DataType::Current, DataType::Forecast, and
  // DataType::CurrentAndForecast) are supported.
  return (data != DataType::none);
}

#ifndef wic_no_json_parsing
bool DarkSky::parseCurrentWeather(const std::string& json, Weather& weather) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonDarkSky::parseCurrentWeather(json, weather);
#else
  return NLohmannJsonDarkSky::parseCurrentWeather(json, weather);
#endif // __SIZEOF_INT128__
}

bool DarkSky::parseForecast(const std::string& json, Forecast& forecast) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonDarkSky::parseForecast(json, forecast);
#else
  return NLohmannJsonDarkSky::parseForecast(json, forecast);
#endif // __SIZEOF_INT128__
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string DarkSky::toRequestString(const Location& location) const
{
  if (location.hasCoordinates())
    return std::to_string(location.latitude())+ std::string(",")
         + std::to_string(location.longitude());
  // no required data set
  return std::string();
}

bool DarkSky::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.darksky.net/forecast/" + m_apiKey
                        + "/" + toRequestString(location) + "?units=si"
                        + "&exclude=minutely";
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
      std::cerr << "Error in DarkSky::currentWeather(): Unexpected HTTP status code "
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

  // Parsing is done here.
  return parseCurrentWeather(response, weather);
}

bool DarkSky::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.darksky.net/forecast/" + m_apiKey
                        + "/" + toRequestString(location) + "?units=si"
                        + "&exclude=minutely";
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
      std::cerr << "Error in DarkSky::forecastWeather(): Unexpected HTTP status code "
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

  // Parsing is done in another method.
  return parseForecast(response, forecast);
}

bool DarkSky::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  weather = Weather();
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.darksky.net/forecast/" + m_apiKey
                        + "/" + toRequestString(location) + "?units=si"
                        + "&exclude=minutely";
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    forecast.setRequestTime(std::chrono::system_clock::now());
    weather.setRequestTime(forecast.requestTime());
    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in DarkSky::currentAndForecastWeather(): Unexpected HTTP status code "
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

  // Parse current weather and forecast with the already existing functions.
  if (!parseCurrentWeather(response, weather))
    return false;
  return parseForecast(response, forecast);
}
#endif // wic_no_network_requests

} // namespace
