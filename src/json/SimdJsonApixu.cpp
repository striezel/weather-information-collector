/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2020  Dirk Stolle

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

#include "SimdJsonApixu.hpp"
#include <iostream>
#include "ApixuFunctions.hpp"

namespace wic
{

bool SimdJsonApixu::parseCurrentWeather(const std::string& json, Weather& weather)
{
  simdjson::dom::parser parser;
  const auto [doc, parseError] = parser.parse(json);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonApixu::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  auto [current, error] = doc["current"];
  if (!error && current.type() == simdjson::dom::element_type::OBJECT)
  {
    // temperature
    simdjson::dom::element v2;
    current["temp_c"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      weather.setTemperatureCelsius(v2.get<double>().value());
    }
    current["temp_f"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      weather.setTemperatureFahrenheit(v2.get<double>().value());
    }
    // wind
    current["wind_degree"].tie(v2, error);
    if (!error && v2.is<int64_t>())
      weather.setWindDegrees(v2.get<int64_t>());
    current["wind_kph"].tie(v2, error);
    if (!error && v2.is<double>())
      weather.setWindSpeed(v2.get<double>().value() / 3.6);
    else
    {
      current["wind_mph"].tie(v2, error);
      if (!error && v2.is<double>())
        weather.setWindSpeed(v2.get<double>().value() * 1.609344 / 3.6);
    }
    // humidity
    current["humidity"].tie(v2, error);
    if (!error && v2.is<int64_t>())
      weather.setHumidity(v2.get<int64_t>().value());
    // rain or snow
    current["precip_mm"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      const float amount = v2.get<double>().value();
      precipitationDistinction(amount, weather);
    }
    // pressure
    current["pressure_mb"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      weather.setPressure(v2.get<double>().value());
    }
    // cloudiness
    current["cloud"].tie(v2, error);
    if (!error && v2.is<int64_t>())
      weather.setCloudiness(v2.get<int64_t>().value());
    // date of data update
    current["last_updated_epoch"].tie(v2, error);
    if (!error && v2.is<int64_t>())
    {
      const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2.get<int64_t>().value()));
      weather.setDataTime(dt);
    }
    return true;
  } // if current object

  // No current object - return false to indicate failure.
  return false;
}

bool SimdJsonApixu::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  const auto [doc, parseError] = parser.parse(json);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);

  const auto [jsForecast, forecastError] = doc["forecast"];
  if (forecastError || jsForecast.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): forecast element is missing!" << std::endl;
    return false;
  }
  auto [forecastday, error] = jsForecast["forecastday"];
  // forecastday must be a non-empty array.
  if (error || forecastday.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): forecastday element is missing!" << std::endl;
    return false;
  }
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type& value : forecastday)
  {
    Weather w_min;
    const auto [date_epoch, err] = value["date_epoch"];
    if (err || !date_epoch.is<int64_t>())
    {
      std::cerr << "Error in SimdJsonApixu::parseForecast(): date_epoch element is missing or not an integer!" << std::endl;
      return false;
    }
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch.get<int64_t>().value()));
    w_min.setDataTime(dt);
    auto [hour, errorHour] = value["hour"];
    if (!errorHour && hour.type() == simdjson::dom::element_type::ARRAY)
    {
      // hourly data is present, use that.
      simdjson::dom::element v2;
      for (const value_type& elem: hour)
      {
        Weather w;
        elem["time_epoch"].tie(v2, error);
        if (error || !v2.is<int64_t>())
        {
          std::cerr << "Error in SimdJsonApixu::parseForecast(): Hourly forecast data has no timestamp!" << std::endl;
          return false;
        }
        w.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2.get<int64_t>().value())));
        // temperature
        elem["temp_c"].tie(v2, error);
        if (!error && v2.is<double>())
          w.setTemperatureCelsius(v2.get<double>());
        elem["temp_f"].tie(v2, error);
        if (!error && v2.is<double>())
          w.setTemperatureFahrenheit(v2.get<double>());
        // wind
        elem["wind_kph"].tie(v2, error);
        if (!error && v2.is<double>())
          w.setWindSpeed(v2.get<double>() / 3.6);
        elem["wind_degree"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setWindDegrees(v2.get<int64_t>());
        // air pressure: pressure_mb
        elem["pressure_mb"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setPressure(v2.get<int64_t>().value());
        // rain or snow: precip_mm
        elem["precip_mm"].tie(v2, error);
        if (!error && v2.is<double>())
        {
          const float amount = v2.get<double>().value();
          precipitationDistinction(amount, w);
        }
        // humidity
        elem["humidity"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setHumidity(v2.get<int64_t>().value());
        // cloudiness
        elem["cloud"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setCloudiness(v2.get<int64_t>().value());
        // Push data of current element onto result.
        data.push_back(w);
      } // for (range-based)
    } // if (hourly data)
    else
    {
      auto [day, errorDay] = value["day"];
      // day must be a non-empty object.
      if (errorDay || day.type() != simdjson::dom::element_type::OBJECT)
      {
        std::cout << "Error in SimdJsonApixu::parseForecast(): JSON element 'day' is empty or not an object!" << std::endl;
        return false;
      }
      simdjson::dom::element v2;
      day["mintemp_c"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_min.setTemperatureCelsius(v2.get<double>().value());
      }
      day["mintemp_f"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_min.setTemperatureFahrenheit(v2.get<double>().value());
      }
      const auto [avghumidity, errorHum] = day["avghumidity"];
      if (!errorHum && avghumidity.is<double>())
        w_min.setHumidity(avghumidity.get<double>().value());
      const auto [totalprecip_mm, errorPrecip] = day["totalprecip_mm"];
      if (!errorPrecip && totalprecip_mm.is<double>())
      {
        const float amount = totalprecip_mm.get<double>().value();
        precipitationDistinction(amount, w_min);
      }
      data.push_back(w_min);

      Weather w_max;
      w_max.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch.get<int64_t>().value() + 3600 * 12)));
      day["maxtemp_c"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_max.setTemperatureCelsius(v2.get<double>().value());
      }
      day["maxtemp_f"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_max.setTemperatureFahrenheit(v2.get<double>().value());
      }
      if (!errorHum && avghumidity.is<double>())
        w_max.setHumidity(avghumidity.get<double>().value());
      if (!errorPrecip && totalprecip_mm.is<double>())
      {
        const float amount = totalprecip_mm.get<double>().value();
        precipitationDistinction(amount, w_max);
      }
      data.push_back(w_max);
    } // else (daily data)
  } // for (range-based)

  forecast.setData(data);
  return true;
}

} // namespace
