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

#include "NLohmannJsonWeatherstack.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool NLohmannJsonWeatherstack::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonWeatherstack::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  if (root.empty())
    return false;
  auto find = root.find("current");
  if (find != root.end() && find->is_object())
  {
    const value_type current = *find;
    // temperature
    auto v2 = current.find("temperature");
    if (v2 != current.end() && v2->is_number())
    {
      weather.setTemperatureCelsius(v2->get<double>());
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
    // wind
    v2 = current.find("wind_degree");
    if (v2 != current.end() && v2->is_number_integer())
      weather.setWindDegrees(v2->get<int>());
    v2 = current.find("wind_speed");
    if (v2 != current.end() && v2->is_number())
      weather.setWindSpeed(v2->get<double>() / 3.6);
    // humidity
    v2 = current.find("humidity");
    if (v2 != current.end() && v2->is_number_integer())
      weather.setHumidity(v2->get<int>());
    // pressure
    v2 = current.find("pressure");
    if (v2 != current.end() && v2->is_number())
    {
      weather.setPressure(v2->get<double>());
    }
    // rain (or snow?)
    v2 = current.find("precip");
    if (v2 != current.end() && v2->is_number())
    {
      weather.setRain(v2->get<float>());
    }
    //weather.setSnow(0.0f);

    // cloudiness
    v2 = current.find("cloudcover");
    if (v2 != current.end() && v2->is_number_integer())
      weather.setCloudiness(v2->get<int>());
    // date of data update
    const auto location = root.find("location");
    if (location != root.end() && location->is_object())
    {
      v2 = location->find("localtime_epoch");
      if (v2 != location->end() && v2->is_number_integer())
      {
        const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2->get<int64_t>()));
        weather.setDataTime(dt);
        return true;
      }
      else
      {
        std::cerr << "Error in NLohmannJsonWeatherstack::parseCurrentWeather(): location object does not have an integral localtime_epoch entry!" << std::endl;
        return false;
      }
    }
    else
    {
      std::cerr << "Error in NLohmannJsonWeatherstack::parseCurrentWeather(): JSON does not have a location object!" << std::endl;
      return false;
    }
  } // if current object

  // No current object - return false to indicate failure.
  return false;
}

bool NLohmannJsonWeatherstack::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonWeatherstack::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  // TODO: Implement!
  std::cerr << "Error in NLohmannJsonWeatherstack::parseForecast(): Not implemented!" << std::endl;
  return false;
}

} // namespace
