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

#include "NLohmannJsonWeatherbit.hpp"
#include <iostream>
#include "../util/NumericPrecision.hpp"

namespace wic
{

bool NLohmannJsonWeatherbit::parseSingleWeatherItem(const value_type& value, Weather& weather)
{
  if (value.empty() || !value.is_object())
    return false;
  auto find = value.find("temp");
  if (find != value.end() && find->is_number())
  {
    const float celsiusRaw = find->get<float>();
    weather.setTemperatureCelsius(celsiusRaw);
    // Avoid values like 5.9999... °C by rounding, if appropriate.
    const float celsiusRounded = NumericPrecision<float>::enforce(weather.temperatureCelsius());
    if (celsiusRounded != weather.temperatureCelsius())
    {
      weather.setTemperatureCelsius(celsiusRounded);
    }
    weather.setTemperatureKelvin(celsiusRaw + 273.15);
    // Avoid values like 280.9999... K by rounding, if appropriate.
    const float kelvinRounded = NumericPrecision<float>::enforce(weather.temperatureKelvin());
    if (kelvinRounded != weather.temperatureKelvin())
    {
      weather.setTemperatureKelvin(kelvinRounded);
    }
    weather.setTemperatureFahrenheit(celsiusRaw * 1.8 + 32.0f);
    // Avoid values like 6.9999... ° F by rounding, if appropriate.
    const float fahrenheitRounded = NumericPrecision<float>::enforce(weather.temperatureFahrenheit());
    if (fahrenheitRounded != weather.temperatureFahrenheit())
    {
      weather.setTemperatureFahrenheit(fahrenheitRounded);
    }
  }
  find = value.find("rh");
  if (find != value.end() && find->is_number_unsigned())
    weather.setHumidity(find->get<int>());
  find = value.find("precip");
  if (find != value.end() && find->is_number())
    weather.setRain(find->get<float>());
  find = value.find("snow");
  if (find != value.end() && find->is_number())
    weather.setSnow(find->get<float>());
  find = value.find("pres");
  if (find != value.end() && find->is_number())
    weather.setPressure(static_cast<int16_t>(find->get<float>()));
  find = value.find("wind_spd");
  if (find != value.end() && find->is_number())
    weather.setWindSpeed(find->get<float>());
  find = value.find("wind_dir");
  if (find != value.end() && find->is_number_unsigned())
    weather.setWindDegrees(static_cast<int16_t>(find->get<float>()));
  find = value.find("clouds");
  if (find != value.end() && find->is_number_unsigned())
      weather.setCloudiness(find->get<int>());

  find = value.find("ts");
  if (find != value.end() && find->is_number())
  {
    // I'm not sure whether this really works as expected.
    // Weatherbit's documentation says the timestamp is UTC, but tests show that
    // this may not be the case.
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(find->get<int64_t>()));
    weather.setDataTime(dt);
  } // if ts
  return weather.hasDataTime();
}

bool NLohmannJsonWeatherbit::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonWeatherbit::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  const auto foundData = root.find("data");
  if (foundData == root.end() || !foundData->is_array() || foundData->size() != 1)
  {
    std::cerr << "Error in NLohmannJsonWeatherbit::parseCurrentWeather(): JSON "
              << "does not contain a data element or data element is not an "
              << "array containing a single element!" << std::endl;
    return false;
  }
  const auto count = root.find("count");
  if (count == root.end() || !count->is_number_unsigned())
  {
    std::cerr << "Error in NLohmannJsonWeatherbit::parseCurrentWeather(): JSON does not contain a count element or the element is not a valid number!" << std::endl;
    return false;
  }
  if (count->get<unsigned int>() != 1)
  {
    std::cerr << "Error in NLohmannJsonWeatherbit::parseCurrentWeather(): Expected count to be 1, but the actual count is "
              << count->get<unsigned int>() << "!" << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(foundData->at(0), weather);
}

bool NLohmannJsonWeatherbit::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonWeatherbit::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const auto findData = root.find("data");
  if ((findData == root.end()) || findData->empty() || !findData->is_array())
  {
    std::cerr << "Error in NLohmannJsonWeatherbit::parseForecast(): data is either empty or not an array!" << std::endl;
    return false;
  }
  const value_type dataJson = *findData;
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type& val : dataJson)
  {
    Weather w;
    if (parseSingleWeatherItem(val, w))
    {
      data.push_back(w);
    }
    else
    {
      std::cerr << "Error in NLohmannJsonWeatherbit::parseForecast(): Parsing single item failed!" << std::endl;
      return false;
    }
  } // for (range-based)
  forecast.setData(data);
  return !forecast.data().empty();
}

} // namespace
