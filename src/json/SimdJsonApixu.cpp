/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2020, 2021  Dirk Stolle

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
  simdjson::dom::element doc;
  const auto parseError = parser.parse(json).get(doc);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonApixu::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(parseError) << std::endl;
    return false;
  }

  weather.setJson(json);

  simdjson::dom::element current;
  auto error = doc["current"].get(current);
  if (error || current.type() != simdjson::dom::element_type::OBJECT)
  {
    // No current object - return false to indicate failure.
    return false;
  }

  // temperature
  simdjson::dom::element v2;
  current["temp_c"].tie(v2, error);
  if (!error && v2.is<double>())
  {
    weather.setTemperatureCelsius(static_cast<float>(v2.get<double>().value()));
  }
  current["temp_f"].tie(v2, error);
  if (!error && v2.is<double>())
  {
    weather.setTemperatureFahrenheit(static_cast<float>(v2.get<double>().value()));
  }
  // wind
  current["wind_degree"].tie(v2, error);
  if (!error && v2.is<int64_t>())
    weather.setWindDegrees(static_cast<int16_t>(v2.get<int64_t>()));
  current["wind_kph"].tie(v2, error);
  if (!error && v2.is<double>())
    weather.setWindSpeed(static_cast<float>(v2.get<double>().value() / 3.6));
  else
  {
    current["wind_mph"].tie(v2, error);
    if (!error && v2.is<double>())
      weather.setWindSpeed(static_cast<float>(v2.get<double>().value() * 1.609344 / 3.6));
  }
  // humidity
  current["humidity"].tie(v2, error);
  if (!error && v2.is<int64_t>())
    weather.setHumidity(static_cast<int8_t>(v2.get<int64_t>().value()));
  // rain or snow
  current["precip_mm"].tie(v2, error);
  if (!error && v2.is<double>())
  {
    const float amount = static_cast<float>(v2.get<double>().value());
    precipitationDistinction(amount, weather);
  }
  // pressure
  current["pressure_mb"].tie(v2, error);
  if (!error && v2.is<double>())
  {
    weather.setPressure(static_cast<int16_t>(v2.get<double>().value()));
  }
  // cloudiness
  current["cloud"].tie(v2, error);
  if (!error && v2.is<int64_t>())
    weather.setCloudiness(static_cast<int8_t>(v2.get<int64_t>().value()));
  // date of data update
  current["last_updated_epoch"].tie(v2, error);
  if (!error && v2.is<int64_t>())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2.get<int64_t>().value()));
    weather.setDataTime(dt);
  }
  return true;
}

bool SimdJsonApixu::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto parseError = parser.parse(json).get(doc);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(parseError) << std::endl;
    return false;
  }

  forecast.setJson(json);

  simdjson::dom::element jsForecast;
  const auto forecastError = doc["forecast"].get(jsForecast);
  if (forecastError || jsForecast.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): forecast element is missing or it is not an object!" << std::endl;
    return false;
  }
  simdjson::dom::element forecastday;
  auto error = jsForecast["forecastday"].get(forecastday);
  // forecastday must be a non-empty array.
  if (error || forecastday.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): forecastday element is missing or it is not an array!" << std::endl;
    return false;
  }
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type& value : forecastday)
  {
    Weather w_min;
    simdjson::dom::element date_epoch;
    const auto err = value["date_epoch"].get(date_epoch);
    if (err || !date_epoch.is<int64_t>())
    {
      std::cerr << "Error in SimdJsonApixu::parseForecast(): date_epoch element is missing or not an integer!" << std::endl;
      return false;
    }
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch.get<int64_t>().value()));
    w_min.setDataTime(dt);
    simdjson::dom::element hour;
    auto errorHour = value["hour"].get(hour);
    if (!errorHour)
    {
      // If hour is present but not an array, then it is an error.
      if (hour.type() != simdjson::dom::element_type::ARRAY)
      {
        std::cerr << "Error in SimdJsonApixu::parseForecast(): hour element is not an array!" << std::endl;
        return false;
      }
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
          w.setTemperatureCelsius(static_cast<float>(v2.get<double>()));
        elem["temp_f"].tie(v2, error);
        if (!error && v2.is<double>())
          w.setTemperatureFahrenheit(static_cast<float>(v2.get<double>()));
        // wind
        elem["wind_kph"].tie(v2, error);
        if (!error && v2.is<double>())
          w.setWindSpeed(static_cast<float>(v2.get<double>() / 3.6));
        elem["wind_degree"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setWindDegrees(static_cast<int16_t>(v2.get<int64_t>()));
        // air pressure: pressure_mb
        elem["pressure_mb"].tie(v2, error);
        if (!error && v2.is<double>())
          w.setPressure(static_cast<int16_t>(v2.get<double>().value()));
        // rain or snow: precip_mm
        elem["precip_mm"].tie(v2, error);
        if (!error && v2.is<double>())
        {
          const float amount = static_cast<float>(v2.get<double>().value());
          precipitationDistinction(amount, w);
        }
        // humidity
        elem["humidity"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setHumidity(static_cast<int8_t>(v2.get<int64_t>().value()));
        // cloudiness
        elem["cloud"].tie(v2, error);
        if (!error && v2.is<int64_t>())
          w.setCloudiness(static_cast<int8_t>(v2.get<int64_t>().value()));
        // Push data of current element onto result.
        data.push_back(w);
      } // for (range-based)
    } // if (hourly data)
    else
    {
      simdjson::dom::element day;
      auto errorDay = value["day"].get(day);
      // day must be a non-empty object.
      if (errorDay || day.type() != simdjson::dom::element_type::OBJECT)
      {
        std::cerr << "Error in SimdJsonApixu::parseForecast(): JSON element 'day' is missing or not an object!" << std::endl;
        return false;
      }
      simdjson::dom::element v2;
      day["mintemp_c"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_min.setTemperatureCelsius(static_cast<float>(v2.get<double>().value()));
      }
      day["mintemp_f"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_min.setTemperatureFahrenheit(static_cast<float>(v2.get<double>().value()));
      }
      simdjson::dom::element avghumidity;
      const auto errorHum = day["avghumidity"].get(avghumidity);
      if (!errorHum && avghumidity.is<double>())
        w_min.setHumidity(static_cast<int8_t>(avghumidity.get<double>().value()));
      simdjson::dom::element totalprecip_mm;
      const auto errorPrecip = day["totalprecip_mm"].get(totalprecip_mm);
      if (!errorPrecip && totalprecip_mm.is<double>())
      {
        const float amount = static_cast<float>(totalprecip_mm.get<double>().value());
        precipitationDistinction(amount, w_min);
      }
      data.push_back(w_min);

      Weather w_max;
      w_max.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch.get<int64_t>().value() + 3600 * 12)));
      day["maxtemp_c"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_max.setTemperatureCelsius(static_cast<float>(v2.get<double>().value()));
      }
      day["maxtemp_f"].tie(v2, error);
      if (!error && v2.is<double>())
      {
        w_max.setTemperatureFahrenheit(static_cast<float>(v2.get<double>().value()));
      }
      if (!errorHum && avghumidity.is<double>())
        w_max.setHumidity(static_cast<int8_t>(avghumidity.get<double>().value()));
      if (!errorPrecip && totalprecip_mm.is<double>())
      {
        const float amount = static_cast<float>(totalprecip_mm.get<double>().value());
        precipitationDistinction(amount, w_max);
      }
      data.push_back(w_max);
    } // else (daily data)
  } // for (range-based)

  if (data.empty())
  {
    std::cerr << "Error in SimdJsonApixu::parseForecast(): JSON does not contain any forecast data!" << std::endl;
    return false;
  }

  forecast.setData(data);
  return true;
}

} // namespace
