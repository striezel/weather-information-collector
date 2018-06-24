/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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
#include <jsoncpp/json/reader.h>
#include "../net/Curly.hpp"
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

std::string DarkSky::toRequestString(const Location& location) const
{
  if (location.hasCoordinates())
    return std::to_string(location.latitude())+ std::string(",")
         + std::to_string(location.longitude());
  // no required data set
  return std::string();
}

bool DarkSky::parseCurrentWeather(const std::string& json, Weather& weather) const
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in DarkSky::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);
  weather.setRequestTime(std::chrono::system_clock::now());

  if (root.empty())
    return false;

  // Current weather data is located in the currently object below the root.
  const Json::Value currently = root["currently"];
  if (currently.empty() || !currently.isObject())
    return false;

  Json::Value val = currently["time"];
  if (!val.empty() && val.isIntegral())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(val.asInt()));
    weather.setDataTime(dt);
  }
  // temperature (in °C)
  val = currently["temperature"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    weather.setTemperatureCelsius(val.asDouble());
    // Since there are no other values (Fahrenheit or Kelvin), we can just
    // calculate them on the fly.
    weather.setTemperatureFahrenheit(weather.temperatureCelsius() * 1.8 + 32);
    weather.setTemperatureKelvin(weather.temperatureCelsius() + 273.15);
  }
  // relative humidity, [0;1]
  val = currently["humidity"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const long int humidity = std::lround(val.asDouble() * 100);
    weather.setHumidity(humidity);
  }
  // rain (mm/m² in an hour)
  val = currently["precipIntensity"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    weather.setRain(val.asDouble());
  }
  // pressure [hPa]
  val = currently["pressure"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const int16_t press = std::lround(val.asDouble());
    weather.setPressure(press);
  }
  // wind speed [m/s]
  val = currently["windSpeed"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    weather.setWindSpeed(val.asDouble());
  }
  // wind windBearing [°], with 0=N,90=E,180=S,270=W
  val = currently["windBearing"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const int16_t degrees = std::lround(val.asDouble());
    weather.setWindDegrees(degrees);
  }
  // cloud cover, [0;1]
  val = currently["cloudCover"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    long int cloudCover = std::min(101l, std::lround(val.asDouble() * 100));
    cloudCover = std::max(-1l, cloudCover);
    weather.setCloudiness(cloudCover);
  }
  return weather.hasDataTime() && weather.hasTemperatureCelsius() && weather.hasHumidity();
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

} //namespace
