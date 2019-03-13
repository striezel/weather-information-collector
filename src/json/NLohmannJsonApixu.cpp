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

#include "NLohmannJsonApixu.hpp"
#include <iostream>
#include "ApixuFunctions.hpp"

namespace wic
{

bool NLohmannJsonApixu::parseCurrentWeather(const std::string& json, Weather& weather)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonApixu::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  if (root.empty())
    return false;
  auto find = root.find("current");
  if (find != root.end() && find->is_object())
  {
    const value_type val = *find;
    // temperature
    auto v2 = val.find("temp_c");
    if (v2 != val.end() && v2->is_number())
    {
      weather.setTemperatureCelsius(v2->get<double>());
    }
    v2 = val.find("temp_f");
    if (v2 != val.end() && v2->is_number())
    {
      weather.setTemperatureFahrenheit(v2->get<double>());
    }
    // wind
    v2 = val.find("wind_degree");
    if (v2 != val.end() && v2->is_number_integer())
      weather.setWindDegrees(v2->get<int>());
    v2 = val.find("wind_kph");
    if (v2 != val.end() && v2->is_number_float())
      weather.setWindSpeed(v2->get<double>() / 3.6);
    else
    {
      v2 = val.find("wind_mph");
      if (v2 != val.end() && v2->is_number_float())
        weather.setWindSpeed(v2->get<double>() * 1.609344 / 3.6);
    }
    // humidity
    v2 = val.find("humidity");
    if (v2 != val.end() && v2->is_number_integer())
      weather.setHumidity(v2->get<int>());
    // rain or snow
    v2 = val.find("precip_mm");
    if (v2 != val.end() && v2->is_number())
    {
      const float amount = v2->get<float>();
      precipitationDistinction(amount, weather);
    }
    // pressure
    v2 = val.find("pressure_mb");
    if (v2 != val.end() && v2->is_number())
    {
      weather.setPressure(v2->get<double>());
    }
    // cloudiness
    v2 = val.find("cloud");
    if (v2 != val.end() && v2->is_number_integer())
      weather.setCloudiness(v2->get<int>());
    // date of data update
    v2 = val.find("last_updated_epoch");
    if (v2 != val.end() && v2->is_number_integer())
    {
      const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2->get<int64_t>()));
      weather.setDataTime(dt);
    }
    return true;
  } // if current object

  // No current object - return false to indicate failure.
  return false;
}

bool NLohmannJsonApixu::parseForecast(const std::string& json, Forecast& forecast)
{
  value_type root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(...)
  {
    std::cerr << "Error in NLohmannJsonApixu::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const auto find = root.find("forecast");
  if (find == root.end() || find->empty())
  {
    std::cerr << "Error in NLohmannJsonApixu::parseForecast(): forecast element is empty!" << std::endl;
    return false;
  }
  const value_type jsForecast = *find;
  const auto findFcDay = jsForecast.find("forecastday");
  // forecastday must be a non-empty array.
  if (findFcDay == jsForecast.end() || !findFcDay->is_array())
  {
    std::cerr << "Error in NLohmannJsonApixu::parseForecast(): forecastday element is empty!" << std::endl;
    return false;
  }
  const value_type forecastday = *findFcDay;
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type& value : forecastday)
  {
    Weather w_min;
    const auto date_epoch = value.find("date_epoch");
    if (date_epoch == value.end() || !date_epoch->is_number_integer())
    {
      std::cerr << "Error in NLohmannJsonApixu::parseForecast(): date_epoch element is empty or not an integer!" << std::endl;
      return false;
    }
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch->get<int64_t>()));
    w_min.setDataTime(dt);
    auto findHour = value.find("hour");
    if (findHour != value.end() && findHour->is_array())
    {
      // hourly data is present, use that.
      const value_type hour = *findHour;
      for (const value_type& elem: hour)
      {
        Weather w;
        auto found = elem.find("time_epoch");
        if (found == elem.end() || !found->is_number_integer())
        {
          std::cerr << "Error in NLohmannJsonApixu::parseForecast(): Hourly forecast data has no timestamp!" << std::endl;
          return false;
        }
        w.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(found->get<int64_t>())));
        // temperature
        found = elem.find("temp_c");
        if (found != elem.end() && found->is_number())
          w.setTemperatureCelsius(found->get<float>());
        found = elem.find("temp_f");
        if (found != elem.end() && found->is_number())
          w.setTemperatureFahrenheit(found->get<float>());
        // wind
        found = elem.find("wind_kph");
        if (found != elem.end() && found->is_number())
          w.setWindSpeed(found->get<float>() / 3.6f);
        found = elem.find("wind_degree");
        if (found != elem.end() && found->is_number())
          w.setWindDegrees(found->get<int>());
        // air pressure: pressure_mb
        found = elem.find("pressure_mb");
        if (found != elem.end() && found->is_number())
          w.setPressure(found->get<int>());
        // rain or snow: precip_mm
        found = elem.find("precip_mm");
        if (found != elem.end() && found->is_number())
        {
          const float amount = found->get<float>();
          precipitationDistinction(amount, w);
        }
        // humidity
        found = elem.find("humidity");
        if (found != elem.end() && found->is_number())
          w.setHumidity(found->get<int>());
        // cloudiness
        found = elem.find("cloud");
        if (found != elem.end() && found->is_number())
          w.setCloudiness(found->get<int>());
        // Push data of current element onto result.
        data.push_back(w);
      } // for (range-based)
    } // if (hourly data)
    else
    {
      auto found = value.find("day");
      // day must be a non-empty object.
      if (found == value.end() || !found->is_object())
      {
        std::cout << "Error in NLohmannJsonApixu::parseForecast(): JSON element 'day' is empty or not an object!" << std::endl;
        return false;
      }
      const value_type day = *found;
      found = day.find("mintemp_c");
      if (found != day.end() && found->is_number())
      {
        w_min.setTemperatureCelsius(found->get<float>());
      }
      found = day.find("mintemp_f");
      if (found != day.end() && found->is_number())
      {
        w_min.setTemperatureFahrenheit(found->get<float>());
      }
      const auto avghumidity = day.find("avghumidity");
      if (avghumidity != day.end() && avghumidity->is_number())
        w_min.setHumidity(avghumidity->get<int>());
      const auto totalprecip_mm = day.find("totalprecip_mm");
      if (totalprecip_mm != day.end() && totalprecip_mm->is_number())
      {
        const float amount = totalprecip_mm->get<float>();
        precipitationDistinction(amount, w_min);
      }
      data.push_back(w_min);

      Weather w_max;
      w_max.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch->get<int>() + 3600 * 12)));
      found = day.find("maxtemp_c");
      if (found != day.end() && found->is_number())
      {
        w_max.setTemperatureCelsius(found->get<float>());
      }
      found = day.find("maxtemp_f");
      if (found != day.end() && found->is_number())
      {
        w_max.setTemperatureFahrenheit(found->get<float>());
      }
      if (avghumidity != day.end() && avghumidity->is_number())
        w_max.setHumidity(avghumidity->get<int>());
      if (totalprecip_mm != day.end() && totalprecip_mm->is_number())
      {
        const float amount = totalprecip_mm->get<float>();
        precipitationDistinction(amount, w_max);
      }
      data.push_back(w_max);
    } // else (daily data)
  } // for (range-based)

  forecast.setData(data);
  return true;
}

} // namespace
