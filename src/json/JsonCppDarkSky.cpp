/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019  Dirk Stolle

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

#include "JsonCppDarkSky.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool JsonCppDarkSky::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in JsonCppDarkSky::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);
  weather.setRequestTime(std::chrono::system_clock::now());

  if (root.empty())
    return false;

  // Current weather data is located in the currently object below the root.
  const value_type& currently = root["currently"];
  return parseSingleWeatherItem(currently, weather);
}

bool JsonCppDarkSky::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in JsonCppDarkSky::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const value_type hourly = root["hourly"];
  if (hourly.empty() || !hourly.isObject())
  {
    std::cerr << "Error in JsonCppDarkSky::parseForecast(): The element \"hourly\" is missing or is not an object!" << std::endl;
    return false;
  }
  const value_type hourlyData = hourly["data"];
  if (hourlyData.empty() || !hourlyData.isArray())
  {
    std::cerr << "Error in JsonCppDarkSky::parseForecast(): The element \"data\" is missing or is not an array!" << std::endl;
    return false;
  }

  forecast.setData({ });
  auto data = forecast.data();
  for (const Json::Value& val : hourlyData)
  {
    Weather w;
    if (!parseSingleWeatherItem(val, w))
    {
      std::cerr << "Error in JsonCppDarkSky::parseForecast(): Parsing of element in data array failed!" << std::endl;
      return false;
    }
    data.push_back(w);
  } // for (range-based)
  // Set data to parsed data ...
  forecast.setData(data);
  // And we are done here.
  return true;
}

bool JsonCppDarkSky::parseSingleWeatherItem(const value_type& dataPoint, Weather& weather)
{
  if (dataPoint.empty() || !dataPoint.isObject())
    return false;

  value_type val = dataPoint["time"];
  if (!val.empty() && val.isIntegral())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(val.asInt()));
    weather.setDataTime(dt);
  }
  // temperature (in °C)
  val = dataPoint["temperature"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    weather.setTemperatureCelsius(val.asDouble());
    // Since there are no other values (Fahrenheit or Kelvin), we can just
    // calculate them on the fly.
    weather.setTemperatureFahrenheit(weather.temperatureCelsius() * 1.8 + 32);
    // Avoid values like 6.9999... ° F by rounding, if appropriate.
    const float fahrenheitRounded = std::round(weather.temperatureFahrenheit());
    if (std::fabs(fahrenheitRounded - weather.temperatureFahrenheit()) < 0.005)
    {
      weather.setTemperatureFahrenheit(fahrenheitRounded);
    }
    weather.setTemperatureKelvin(weather.temperatureCelsius() + 273.15);
  }
  // relative humidity, [0;1]
  val = dataPoint["humidity"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const long int humidity = std::lround(val.asDouble() * 100);
    weather.setHumidity(humidity);
  }
  // rain or snow (mm/m² in an hour)
  val = dataPoint["precipIntensity"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const double amount = val.asDouble();
    const value_type& type = dataPoint["precipType"];
    if (!type.empty() && type.isString())
    {
      const auto typeStr = type.asString();
      if (typeStr == "rain")
      {
        weather.setRain(amount);
        weather.setSnow(0.0);
      }
      else if (typeStr == "snow")
      {
        weather.setRain(0.0);
        weather.setSnow(amount);
      }
      else if (typeStr == "sleet")
      {
        // Sleet could be described as mixture of snow and rain.
        weather.setRain(amount);
        weather.setSnow(amount);
      }
      else
      {
        std::cerr << "Error in JsonCppDarkSky::parseCurrentWeather(): Unknown precipType "
                  << typeStr << "!" << std::endl;
        return false;
      }
    }
    // precipType may not be present, if precipIntensity is zero.
    else if (amount == 0.0)
    {
      weather.setRain(0.0);
      weather.setSnow(0.0);
    }
    else
    {
      std::cerr << "Error in JsonCppDarkSky::parseCurrentWeather(): Missing precipType!"
                << std::endl;
      return false;
    }
  } // if precipIntensity
  // pressure [hPa]
  val = dataPoint["pressure"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const int16_t press = std::lround(val.asDouble());
    weather.setPressure(press);
  }
  // wind speed [m/s]
  val = dataPoint["windSpeed"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    weather.setWindSpeed(val.asDouble());
  }
  // wind windBearing [°], with 0=N,90=E,180=S,270=W
  val = dataPoint["windBearing"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    const int16_t degrees = std::lround(val.asDouble());
    weather.setWindDegrees(degrees);
  }
  // cloud cover, [0;1]
  val = dataPoint["cloudCover"];
  if (!val.empty() && (val.isDouble() || val.isIntegral()))
  {
    long int cloudCover = std::min(101l, std::lround(val.asDouble() * 100));
    cloudCover = std::max(-1l, cloudCover);
    weather.setCloudiness(cloudCover);
  }
  return weather.hasDataTime() && weather.hasTemperatureCelsius() && weather.hasHumidity();
}

} // namespace
