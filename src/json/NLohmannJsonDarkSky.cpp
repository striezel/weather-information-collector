/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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

#include "NLohmannJsonDarkSky.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool NLohmannJsonDarkSky::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonDarkSky::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);
  weather.setRequestTime(std::chrono::system_clock::now());

  if (root.empty())
    return false;

  // Current weather data is located in the currently object below the root.
  const auto find = root.find("currently");
  if (find == root.end())
  {
    std::cerr << "Error in NLohmannJsonDarkSky::parseCurrentWeather(): No JSON element \"currently\" found!" << std::endl;
    return false;
  }
  const value_type& currently = *find;
  return parseSingleWeatherItem(currently, weather);
}

bool NLohmannJsonDarkSky::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonDarkSky::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  auto find = root.find("hourly");
  if (find == root.end() || !find->is_object())
  {
    std::cerr << "Error in NLohmannJsonDarkSky::parseForecast(): The element \"hourly\" is missing or is not an object!" << std::endl;
    return false;
  }
  const value_type hourly = *find;
  const auto findData = hourly.find("data");
  if (findData ==  hourly.end() || !findData->is_array())
  {
    std::cerr << "Error in NLohmannJsonDarkSky::parseForecast(): The element \"data\" is missing or is not an array!" << std::endl;
    return false;
  }
  const value_type hourlyData = *find;

  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type& val : hourlyData)
  {
    Weather w;
    if (!parseSingleWeatherItem(val, w))
    {
      std::cerr << "Error in NLohmannJsonDarkSky::parseForecast(): Parsing of element in data array failed!" << std::endl;
      return false;
    }
    data.push_back(w);
  } // for (range-based)
  // Set data to parsed data ...
  forecast.setData(data);
  // And we are done here.
  return true;
}

bool NLohmannJsonDarkSky::parseSingleWeatherItem(const value_type& dataPoint, Weather& weather)
{
  if (dataPoint.empty() || !dataPoint.is_object())
    return false;

  auto find = dataPoint.find("time");
  if (find != dataPoint.end() && find->is_number_integer())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(find->get<int64_t>()));
    weather.setDataTime(dt);
  }
  // temperature (in °C)
  find = dataPoint.find("temperature");
  if (find != dataPoint.end() && find->is_number())
  {
    weather.setTemperatureCelsius(find->get<double>());
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
  find = dataPoint.find("humidity");
  if (find != dataPoint.end() && find->is_number())
  {
    const long int humidity = std::lround(find->get<double>() * 100);
    weather.setHumidity(humidity);
  }
  // rain or snow (mm/m² in an hour)
  find = dataPoint.find("precipIntensity");
  if (find != dataPoint.end() && find->is_number())
  {
    const double amount = find->get<double>();
    const auto findType = dataPoint.find("precipType");
    if (findType != dataPoint.end() && findType->is_string())
    {
      const std::string typeStr = findType->get<std::string>();
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
        std::cerr << "Error in NLohmannJsonDarkSky::parseCurrentWeather(): Unknown precipType "
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
      std::cerr << "Error in NLohmannJsonDarkSky::parseCurrentWeather(): Missing precipType!"
                << std::endl;
      return false;
    }
  } // if precipIntensity
  // pressure [hPa]
  find = dataPoint.find("pressure");
  if (find != dataPoint.end() && find->is_number())
  {
    const int16_t press = std::lround(find->get<double>());
    weather.setPressure(press);
  }
  // wind speed [m/s]
  find = dataPoint.find("windSpeed");
  if (find != dataPoint.end() && find->is_number())
  {
    weather.setWindSpeed(find->get<double>());
  }
  // wind windBearing [°], with 0=N,90=E,180=S,270=W
  find = dataPoint.find("windBearing");
  if (find != dataPoint.end() && find->is_number())
  {
    const int16_t degrees = std::lround(find->get<double>());
    weather.setWindDegrees(degrees);
  }
  // cloud cover, [0;1]
  find = dataPoint.find("cloudCover");
  if (find != dataPoint.end() && find->is_number())
  {
    long int cloudCover = std::min(101l, std::lround(find->get<double>() * 100));
    cloudCover = std::max(-1l, cloudCover);
    weather.setCloudiness(cloudCover);
  }
  return weather.hasDataTime() && weather.hasTemperatureCelsius() && weather.hasHumidity();
}

} // namespace
