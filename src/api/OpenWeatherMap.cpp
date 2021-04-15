/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021  Dirk Stolle

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

#include "OpenWeatherMap.hpp"
#include <cmath>
#include <iostream>
#ifdef wic_owm_find_location
#include <array>
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonOwm.hpp"
#else
#include "../json/NLohmannJsonOwm.hpp"
#endif // __SIZEOF_INT128__
#include "../net/Request.hpp"
#endif // wic_owm_find_location
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonOwm.hpp"
#else
#include "../json/NLohmannJsonOwm.hpp"
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Request.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

OpenWeatherMap::OpenWeatherMap(const std::string& key)
: m_apiKey(key)
{
}

void OpenWeatherMap::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool OpenWeatherMap::validLocation(const Location& location) const
{
  return (location.hasOwmId() || location.hasCoordinates()
      || location.hasName() || location.hasPostcode());
}

bool OpenWeatherMap::supportsDataType(const DataType data) const
{
  // Only current weather data and forecast can be retrieved, not both together.
  switch (data)
  {
    case DataType::Current:
    case DataType::Forecast:
         return true;
    default: // i. e. DataType::none and DataType::CurrentAndForecast
         return false;
  }
}

#ifndef wic_no_json_parsing
bool OpenWeatherMap::parseCurrentWeather(const std::string& json, Weather& weather) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonOwm::parseCurrentWeather(json, weather);
#else
  return NLohmannJsonOwm::parseCurrentWeather(json, weather);
#endif
}

bool OpenWeatherMap::parseForecast(const std::string& json, Forecast& forecast) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonOwm::parseForecast(json, forecast);
#else
  return NLohmannJsonOwm::parseForecast(json, forecast);
#endif
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string OpenWeatherMap::toRequestString(const Location& location) const
{
  if (location.hasOwmId())
    return std::string("id=") + intToString(location.owmId());
  if (location.hasCoordinates())
    return std::string("lat=") + floatToString(location.latitude())
         + std::string("&lon=") + floatToString(location.longitude());
  if (location.hasName())
    return std::string("q=") + location.name();
  if (location.hasPostcode())
    return std::string("zip=") + location.postcode();
  // no required data set
  return std::string();
}

bool OpenWeatherMap::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.openweathermap.org/data/2.5/weather?appid="
                        + m_apiKey + "&" + toRequestString(location);
  weather.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "OpenWeatherMap::currentWeather");
  if (!response.has_value())
    return false;

  // Parsing is done here.
  return parseCurrentWeather(response.value(), weather);
}

bool OpenWeatherMap::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty() || location.empty())
    return false;
  const std::string url = "https://api.openweathermap.org/data/2.5/forecast?appid="
                        + m_apiKey + "&" + toRequestString(location);
  forecast.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "OpenWeatherMap::forecastWeather");
  if (!response.has_value())
    return false;

  // Parsing is done here.
  return parseForecast(response.value(), forecast);
}

bool OpenWeatherMap::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  std::cerr << "Error: Getting both current weather and forecast data in a "
            << "single request is not supported by OpenWeatherMap!" << std::endl;
  return false;
}
#endif // wic_no_network_requests

#ifdef wic_owm_find_location
std::string urlEncode(const std::string& str)
{
  static const std::array<char, 16> hexDigits = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  std::string encoded;
  for (const char c: str)
  {
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z') || (c == '-') || (c == '_')
      || (c == '.') || (c == '~'))
    {
      encoded.push_back(c);
    }
    else
    {
      // d
      encoded.push_back('%');
      encoded.push_back(hexDigits[c / 16]);
      encoded.push_back(hexDigits[c % 16]);
    }
  } // for
  return encoded;
}

bool OpenWeatherMap::findLocation(const std::string& name, std::vector<std::pair<Location, Weather> >& locations) const
{
  if (m_apiKey.empty() || name.empty())
    return false;

  const std::string url = "https://api.openweathermap.org/data/2.5/find?q=" + urlEncode(name) + "&appid=" + m_apiKey;
  const auto response = Request::get(url, "OpenWeatherMap::findLocation");
  if (!response.has_value())
    return false;

#ifdef __SIZEOF_INT128__
  return SimdJsonOwm::parseLocations(response.value(), locations);
#else
  return NLohmannJsonOwm::parseLocations(response.value(), locations);
#endif
}
#endif // wic_owm_find_location

} // namespace
