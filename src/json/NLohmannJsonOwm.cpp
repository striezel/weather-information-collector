/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2021  Dirk Stolle

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
  auto find = value.find("main");
  bool foundValidParts = false;
  if (find != value.end() && find->is_object())
  {
    const value_type main = value["main"];
    find = main.find("temp");
    if (find != main.end() && find->is_number())
    {
      const value_type temp = main["temp"];
      const float kelvinRaw = temp.get<float>();
      weather.setTemperatureKelvin(kelvinRaw);
      // Avoid values like 280.9999... K by rounding, if appropriate.
      const float kelvinRounded = std::round(weather.temperatureKelvin());
      if (std::fabs(kelvinRounded - weather.temperatureKelvin()) < 0.005)
      {
        weather.setTemperatureKelvin(kelvinRounded);
      }
      weather.setTemperatureCelsius(kelvinRaw - 273.15f);
      // Avoid values like 6.9999... ° C by rounding, if appropriate.
      const float celsiusRounded = std::round(weather.temperatureCelsius());
      if (std::fabs(celsiusRounded - weather.temperatureCelsius()) < 0.005)
      {
        weather.setTemperatureCelsius(celsiusRounded);
      }
      weather.setTemperatureFahrenheit(static_cast<float>((kelvinRaw - 273.15f) * 1.8 + 32.0));
      // Avoid values like 6.9999... ° F by rounding, if appropriate.
      const float fahrenheitRounded = std::round(weather.temperatureFahrenheit());
      if (std::fabs(fahrenheitRounded - weather.temperatureFahrenheit()) < 0.005)
      {
        weather.setTemperatureFahrenheit(fahrenheitRounded);
      }
    }
    find = main.find("pressure");
    if (find != main.end() && find->is_number())
      weather.setPressure(static_cast<int16_t>(find->get<float>()));
    find = main.find("humidity");
    if (find != main.end() && find->is_number_unsigned())
      weather.setHumidity(static_cast<int8_t>(find->get<int>()));
    foundValidParts = true;
  } // if main object
  find = value.find("wind");
  if (find != value.end() && find->is_object())
  {
    const value_type wind = *find;
    find = wind.find("speed");
    if (find != wind.end() && find->is_number())
      weather.setWindSpeed(find->get<float>());
    find = wind.find("deg");
    if (find != wind.end() && find->is_number())
      weather.setWindDegrees(static_cast<int16_t>(find->get<float>()));
  } // if wind object
  find = value.find("clouds");
  if (find != value.end() && find->is_object())
  {
    const value_type clouds = *find;
    find = clouds.find("all");
    if (find != clouds.end() && find->is_number_integer())
      weather.setCloudiness(static_cast<int8_t>(find->get<int>()));
  } // if clouds object
  find = value.find("rain");
  if (find != value.end() && find->is_object())
  {
    const value_type rain = *find;
    find = rain.find("3h");
    if (find != rain.end())
    {
      if (find->is_number())
        weather.setRain(find->get<float>());
    }
    else
    {
      // Empty rain object means zero rain.
      weather.setRain(0.0f);
    }
  } // if rain object
  find = value.find("snow");
  if (find != value.end() && find->is_object())
  {
    const value_type snow = *find;
    const auto find3h = snow.find("3h");
    if (find3h != snow.end())
    {
      if (find3h->is_number())
        weather.setSnow(find3h->get<float>());
    }
    else
    {
      // Empty snow object means zero snow.
      weather.setSnow(0.0f);
    }
  } // if rain object
  find = value.find("dt");
  if (find != value.end() && find->is_number_integer())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(find->get<int64_t>()));
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
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
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
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const auto findList = root.find("list");
  if ((findList == root.end()) || findList->empty() || !findList->is_array())
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): list is either empty or not an array!" << std::endl;
    return false;
  }
  const value_type list = *findList;
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
  const auto findCnt = root.find("cnt");
  if ((findCnt == root.end()) || findCnt->empty() || !findCnt->is_number_unsigned())
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): cnt is empty or not an integer!" << std::endl;
    return false;
  }
  const decltype(data.size()) cnt = findCnt->get<unsigned int>();
  // Number of data items should be the number given in "cnt".
  if (data.size() != cnt)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseForecast(): Expected " << cnt
              << " items, but " << data.size() << " items were found!" << std::endl;
    return false;
  }
  forecast.setData(data);
  return true;
}

#ifdef wic_owm_find_location
bool NLohmannJsonOwm::parseLocations(const std::string& json, std::vector<std::pair<Location, Weather> >& locations)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOwm::parseLocations(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  const auto count = root.find("count");
  if (count == root.end() || !count->is_number_integer())
  {
    std::cerr << "Error in NLohmannJsonOwm::parseLocations(): JSON element for count is missing!" << std::endl;
    return false;
  }
  locations.clear();
  if (count->get<int>() == 0)
    return true;

  const auto listFind = root.find("list");
  if (listFind == root.end() || !listFind->is_array())
  {
    std::cerr << "Error in NLohmannJsonOwm::parseLocations(): JSON list element is missing!" << std::endl;
    return false;
  }
  const value_type list = *listFind;

  for (const value_type elem : list)
  {
    Location loc;
    auto val = elem.find("id");
    if (val != elem.end() && val->is_number_unsigned())
      loc.setOwmId(val->get<unsigned int>());
    val = elem.find("name");
    if (val != elem.end() && val->is_string())
      loc.setName(val->get<std::string>());
    const auto coord = elem.find("coord");
    if (coord != elem.end() && coord->is_object())
    {
      val = coord->find("lat");
      const auto lon = coord->find("lon");
      if (val != coord->end() && lon != coord->end() && val->is_number() && lon->is_number())
      {
        loc.setCoordinates(val->get<float>(), lon->get<float>());
      }
    } // coord
    if (loc.empty())
    {
      std::cerr << "Error in NLohmannJsonOwm::parseLocations(): Location data is empty!" << std::endl;
      return false;
    }
    val = elem.find("sys");
    if (val != elem.end() && val->is_object())
    {
      const auto country = val->find("country");
      if (country != val->end() && country->is_string())
        loc.setCountryCode(country->get<std::string>());
    }
    Weather w;
    if (!NLohmannJsonOwm::parseSingleWeatherItem(elem, w))
    {
      std::cerr << "Error in NLohmannJsonOwm::parseLocations(): Weather data for location is missing!" << std::endl;
      return false;
    }
    if (loc.empty())
    {
      std::cerr << "Error in NLohmannJsonOwm::parseLocations(): Location data is empty!" << std::endl;
      return false;
    }
    // add element to result
    locations.push_back(std::make_pair(loc, w));
  } // for

  return true;
}
#endif // wic_owm_find_location

} // namespace
