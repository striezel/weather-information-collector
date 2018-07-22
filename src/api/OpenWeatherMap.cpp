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

#include "OpenWeatherMap.hpp"
#include <iostream>
#include <jsoncpp/json/reader.h>
#include "../net/Curly.hpp"
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
  return (location.hasId() || location.hasCoordinates()
      || location.hasName() || location.hasPostcode());
}

std::string OpenWeatherMap::toRequestString(const Location& location) const
{
  if (location.hasId())
    return std::string("id=") + intToString(location.id());
  if (location.hasCoordinates())
    return std::string("lat=") + floatToString(location.latitude())
         + std::string("&lon=") + floatToString(location.longitude());
  if (location.hasName())
    return std::string("q=") + location.name();
  if (location.hasPostcode())
    return std::string("zip=") + location.postcode();
  //no required data set
  return std::string();
}

bool OpenWeatherMap::parseCurrentWeather(const std::string& json, Weather& weather) const
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in OpenWeatherMap::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);
  weather.setRequestTime(std::chrono::system_clock::now());

  if (root.empty())
    return false;
  Json::Value val = root["main"];
  bool foundValidParts = false;
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["temp"];
    if (!v2.empty() && v2.isDouble())
    {
      weather.setTemperatureKelvin(v2.asFloat());
      weather.setTemperatureCelsius(weather.temperatureKelvin() - 273.15);
    }
    v2 = val["pressure"];
    if (!v2.empty() && v2.isIntegral())
      weather.setPressure(v2.asInt());
    v2 = val["humidity"];
    if (!v2.empty() && v2.isIntegral())
      weather.setHumidity(v2.asInt());
    foundValidParts = true;;
  } //if main object
  val = root["wind"];
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["speed"];
    if (!v2.empty() && v2.isDouble())
      weather.setWindSpeed(v2.asFloat());
    v2 = val["deg"];
    if (!v2.empty() && v2.isIntegral())
      weather.setWindDegrees(v2.asInt());
  } //if wind object
  val = root["clouds"];
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["all"];
    if (!v2.empty() && v2.isIntegral())
      weather.setCloudiness(v2.asInt());
  } //if clouds object
  val = root["rain"];
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["3h"];
    if (!v2.empty() && (v2.isDouble() || v2.isIntegral()))
      weather.setRain(v2.asFloat());
  } //if rain object
  val = root["dt"];
  if (!val.empty() && val.isIntegral())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(val.asInt()));
    weather.setDataTime(dt);
  } //if dt
  return foundValidParts;
}

bool OpenWeatherMap::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.openweathermap.org/data/2.5/weather?appid="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in OpenWeatherMap::currentWeather(): Unexpected HTTP status code "
                << curly.getResponseCode() << "!" << std::endl;
      const auto & rh = curly.responseHeaders();
      std::cerr << "HTTP response headers (" << rh.size() << "):" << std::endl;
      for (const auto & s : rh)
      {
        std::cerr << "    " << s << std::endl;
      }
      return false;
    }
  } //scope of curly

  //Parsing is done here.
  return parseCurrentWeather(response, weather);
}

} //namespace
