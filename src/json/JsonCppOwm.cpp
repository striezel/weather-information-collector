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

#include "JsonCppOwm.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool JsonCppOwm::parseSingleWeatherItem(const value_type& value, Weather& weather)
{
  if (value.empty())
    return false;
  value_type val = value["main"];
  bool foundValidParts = false;
  if (!val.empty() && val.isObject())
  {
    value_type v2 = val["temp"];
    if (!v2.empty() && v2.isDouble())
    {
      const float kelvinRaw = v2.asFloat();
      weather.setTemperatureKelvin(kelvinRaw);
      // Avoid values like 280.9999... K by rounding, if appropriate.
      const float kelvinRounded = std::round(weather.temperatureKelvin());
      if (std::fabs(kelvinRounded - weather.temperatureKelvin()) < 0.005)
      {
        weather.setTemperatureKelvin(kelvinRounded);
      }
      weather.setTemperatureCelsius(kelvinRaw - 273.15);
      // Avoid values like 6.9999... ° C by rounding, if appropriate.
      const float celsiusRounded = std::round(weather.temperatureCelsius());
      if (std::fabs(celsiusRounded - weather.temperatureCelsius()) < 0.005)
      {
        weather.setTemperatureCelsius(celsiusRounded);
      }
      weather.setTemperatureFahrenheit((kelvinRaw - 273.15) * 1.8 + 32.0f);
      // Avoid values like 6.9999... ° F by rounding, if appropriate.
      const float fahrenheitRounded = std::round(weather.temperatureFahrenheit());
      if (std::fabs(fahrenheitRounded - weather.temperatureFahrenheit()) < 0.005)
      {
        weather.setTemperatureFahrenheit(fahrenheitRounded);
      }
    }
    v2 = val["pressure"];
    if (!v2.empty() && v2.isNumeric())
      weather.setPressure(static_cast<int16_t>(v2.asFloat()));
    v2 = val["humidity"];
    if (!v2.empty() && v2.isIntegral())
      weather.setHumidity(v2.asInt());
    foundValidParts = true;;
  } // if main object
  val = value["wind"];
  if (!val.empty() && val.isObject())
  {
    value_type v2 = val["speed"];
    if (!v2.empty() && v2.isDouble())
      weather.setWindSpeed(v2.asFloat());
    v2 = val["deg"];
    if (!v2.empty() && v2.isNumeric())
      weather.setWindDegrees(static_cast<int16_t>(v2.asFloat()));
  } // if wind object
  val = value["clouds"];
  if (!val.empty() && val.isObject())
  {
    const value_type v2 = val["all"];
    if (!v2.empty() && v2.isIntegral())
      weather.setCloudiness(v2.asInt());
  } // if clouds object
  val = value["rain"];
  if (!val.isNull() && val.isObject())
  {
    const value_type v2 = val["3h"];
    if (!v2.isNull())
    {
      if (v2.isDouble() || v2.isIntegral())
        weather.setRain(v2.asFloat());
    }
    else
    {
      // Empty rain object means zero rain.
      weather.setRain(0.0f);
    }
  } // if rain object
  const Json::Value& snow = value["snow"];
  if (!snow.isNull() && snow.isObject())
  {
    const Json::Value& v2 = snow["3h"];
    if (!v2.isNull())
    {
      if (v2.isDouble() || v2.isIntegral())
        weather.setSnow(v2.asFloat());
    }
    else
    {
      // Empty snow object means zero snow.
      weather.setSnow(0.0f);
    }
  } // if rain object
  val = value["dt"];
  if (!val.empty() && val.isIntegral())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(val.asInt()));
    weather.setDataTime(dt);
  } // if dt
  return foundValidParts;
}

bool JsonCppOwm::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in JsonCppOwm::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(root, weather);
}

bool JsonCppOwm::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in JsonCppOwm::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const value_type list = root["list"];
  if (list.empty() || !list.isArray())
  {
    std::cerr << "Error in JsonCppOwm::parseForecast(): list is either empty or not an array!" << std::endl;
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
      std::cerr << "Error in JsonCppOwm::parseForecast(): Parsing single item failed!" << std::endl;
      return false;
    }
  } // for (range-based)
  const auto val = root["cnt"];
  if (val.empty() || !val.isIntegral())
  {
    std::cerr << "Error in JsonCppOwm::parseForecast(): cnt is empty or not an integer!" << std::endl;
    return false;
  }
  const decltype(data.size()) cnt = val.asUInt();
  // Number of data items should be the number given in "cnt".
  if (data.size() != cnt)
  {
    std::cerr << "Error in JsonCppOwm::parseForecast(): Expected " << cnt
              << " items, but only " << data.size() << " items were found!" << std::endl;
    return false;
  }
  forecast.setData(data);
  return true;
}

#ifdef wic_owm_find_location
bool JsonCppOwm::parseLocations(const std::string& json, std::vector<std::pair<Location, Weather> >& locations)
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in JsonCppOwm::parseLocations(): Unable to parse JSON data!" << std::endl;
    return false;
  }
  const Json::Value count = root["count"];
  if (count.empty() || !count.isIntegral())
  {
    std::cerr << "Error in JsonCppOwm::parseLocations(): JSON element for count is missing!" << std::endl;
    return false;
  }
  locations.clear();
  if (count.asInt() == 0)
    return true;

  const Json::Value list = root["list"];
  if (list.empty() || !list.isArray())
  {
    std::cerr << "Error in JsonCppOwm::parseLocations(): JSON list element is missing!" << std::endl;
    return false;
  }

  for (const Json::Value elem : list)
  {
    Location loc;
    Json::Value val = elem["id"];
    if (!val.empty() && val.isUInt())
      loc.setOwmId(val.asUInt());
    val = elem["name"];
    if (!val.empty() && val.isString())
      loc.setName(val.asString());
    const Json::Value coord = elem["coord"];
    if (!coord.empty() && coord.isObject())
    {
      val = coord["lat"];
      const Json::Value lon = coord["lon"];
      if (!val.empty() && !lon.empty() && val.isNumeric() && lon.isNumeric())
      {
        loc.setCoordinates(val.asFloat(), lon.asFloat());
      }
    } // coord
    if (loc.empty())
    {
      std::cerr << "Error in JsonCppOwm::parseLocations(): Location data is empty!" << std::endl;
      return false;
    }
    val = elem["sys"];
    if (!val.empty() && val.isObject())
    {
      val = val["country"];
      if (!val.empty() && val.isString())
        loc.setCountryCode(val.asString());
    }
    Weather w;
    if (!JsonCppOwm::parseSingleWeatherItem(elem, w))
    {
      std::cerr << "Error in JsonCppOwm::parseLocations(): Weather data for location is missing!" << std::endl;
      return false;
    }
    if (loc.empty())
    {
      std::cerr << "Error in JsonCppOwm::parseLocations(): Location data is empty!" << std::endl;
      return false;
    }
    // add element to result
    locations.push_back(std::make_pair(loc, w));
  } // for

  return true;
}
#endif // wic_owm_find_location

} // namespace
