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

#include "NLohmannJsonOwm.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool NLohmannJsonOwm::parseSingleWeatherItem(const value_type& value, Weather& weather)
{
  if (value.empty())
    return false;
  value_type val = value["main"];
  bool foundValidParts = false;
  if (!val.empty() && val.is_object())
  {
    value_type v2 = val["temp"];
    if (!v2.empty() && v2.is_number())
    {
      weather.setTemperatureKelvin(v2.get<float>());
      weather.setTemperatureCelsius(weather.temperatureKelvin() - 273.15);
      // Avoid values like 6.9999... ° C by rounding, if appropriate.
      const float celsiusRounded = std::round(weather.temperatureCelsius());
      if (std::fabs(celsiusRounded - weather.temperatureCelsius()) < 0.005)
      {
        weather.setTemperatureCelsius(celsiusRounded);
      }
      weather.setTemperatureFahrenheit((weather.temperatureKelvin() - 273.15) * 1.8 + 32.0f);
      // Avoid values like 6.9999... ° F by rounding, if appropriate.
      const float fahrenheitRounded = std::round(weather.temperatureFahrenheit());
      if (std::fabs(fahrenheitRounded - weather.temperatureFahrenheit()) < 0.005)
      {
        weather.setTemperatureFahrenheit(fahrenheitRounded);
      }
    }
    v2 = val["pressure"];
    if (!v2.empty() && v2.is_number())
      weather.setPressure(static_cast<int16_t>(v2.get<float>()));
    v2 = val["humidity"];
    if (!v2.empty() && v2.is_number_unsigned())
      weather.setHumidity(v2.get<int>());
    foundValidParts = true;;
  } // if main object
  val = value["wind"];
  if (!val.empty() && val.is_object())
  {
    value_type v2 = val["speed"];
    if (!v2.empty() && v2.is_number())
      weather.setWindSpeed(v2.get<float>());
    v2 = val["deg"];
    if (!v2.empty() && v2.is_number())
      weather.setWindDegrees(static_cast<int16_t>(v2.get<float>()));
  } // if wind object
  val = value["clouds"];
  if (!val.empty() && val.is_object())
  {
    const value_type v2 = val["all"];
    if (!v2.empty() && v2.is_number_integer())
      weather.setCloudiness(v2.get<int>());
  } // if clouds object
  val = value["rain"];
  if (!val.is_null() && val.is_object())
  {
    const value_type v2 = val["3h"];
    if (!v2.is_null())
    {
      if (v2.is_number())
        weather.setRain(v2.get<float>());
    }
    else
    {
      // Empty rain object means zero rain.
      weather.setRain(0.0f);
    }
  } // if rain object
  const value_type& snow = value["snow"];
  if (!snow.is_null() && snow.is_object())
  {
    const value_type& v2 = snow["3h"];
    if (!v2.is_null())
    {
      if (v2.is_number())
        weather.setSnow(v2.get<float>());
    }
    else
    {
      // Empty snow object means zero snow.
      weather.setSnow(0.0f);
    }
  } // if rain object
  val = value["dt"];
  if (!val.empty() && val.is_number_integer())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(val.get<int64_t>()));
    weather.setDataTime(dt);
  } // if dt
  return foundValidParts;
}

bool NLohmannJsonOwm::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(root, weather);
}

bool NLohmannJsonOwm::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const value_type list = root["list"];
  if (list.empty() || !list.is_array())
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): list is either empty or not an array!" << std::endl;
    return false;
  }
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type val : list)
  {
    Weather w;
    if (parseSingleWeatherItem(val, w))
    {
      data.push_back(w);
    }
    else
    {
      std::cerr << "Error in NLohmannJsonOwm::parseForecast(): Parsing single item failed!" << std::endl;
      return false;
    }
  } // for (range-based)
  const auto val = root["cnt"];
  if (val.empty() || !val.is_number_unsigned())
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): cnt is empty or not an integer!" << std::endl;
    return false;
  }
  const decltype(data.size()) cnt = val.get<unsigned int>();
  // Number of data items should be the number given in "cnt".
  if (data.size() != cnt)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): Expected " << cnt
              << " items, but only " << data.size() << " items were found!" << std::endl;
    return false;
  }
  forecast.setData(data);
  return true;
}

} // namespace
