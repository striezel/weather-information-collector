/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2022  Dirk Stolle

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

#include "NLohmannJsonOpenMeteo.hpp"
#include <iostream>
#include "OpenMeteoFunctions.hpp"

namespace wic
{

bool NLohmannJsonOpenMeteo::parseCurrentWeather(const std::string& json, Weather& weather)
{
  nlohmann::json root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  weather.setJson(json);

  if (root.empty())
    return false;

  // Current weather data is located in the current_weather object below the root.
  auto find = root.find("current_weather");
  if (find == root.end() || !find->is_object())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'current_weather' is either missing or not an object!"
              << std::endl;
    return false;
  }
  nlohmann::json& current_weather = *find;
  find = current_weather.find("temperature");
  if (find == current_weather.end() || !find->is_number())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'temperature' is either missing or not a floating "
              << "point value!" << std::endl;
    return false;
  }
  weather = Weather();
  weather.setTemperatureCelsius(find->get<float>());

  find = current_weather.find("windspeed");
  if (find == current_weather.end() || !find->is_number())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'windspeed' is either missing or not a floating "
              << "point value!" << std::endl;
    return false;
  }
  weather.setWindSpeed(find->get<float>());

  find = current_weather.find("winddirection");
  if (find == current_weather.end() || !find->is_number_unsigned())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'winddirection' is either missing or not an integer!"
              << std::endl;
    return false;
  }
  weather.setWindDegrees(find->get<int>());

  find = current_weather.find("time");
  if (find == current_weather.end() || !find->is_string())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'time' is either missing or not a string!"
              << std::endl;
    return false;
  }
  const auto dt = isoStringToTime(find->get<std::string>());
  if (!dt.has_value())
  {
    return false;
  }
  weather.setDataTime(dt.value());

  // There is no more data in current weather in Open-Meteo.
  weather.setJson(json);
  return true;
}

bool NLohmannJsonOpenMeteo::parseForecast(const std::string& json, Forecast& forecast)
{
  nlohmann::json root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  // TODO: Implement!
  return false;
}

} // namespace
