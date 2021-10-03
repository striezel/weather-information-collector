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

#include "SimdJsonWeatherstack.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "../../third-party/simdjson/simdjson.h"
#include "WeatherstackFunctions.hpp"
#include "../util/NumericPrecision.hpp"

namespace wic
{

std::chrono::time_point<std::chrono::system_clock> parseDateTime(const simdjson::dom::element& root)
{
  // date of data update
  simdjson::dom::element location;
  const auto errorLoc = root["location"].get(location);
  if (!errorLoc && location.type() == simdjson::dom::element_type::OBJECT)
  {
    simdjson::dom::element v1;
    const auto e1 = location["localtime"].get(v1);
    if (!e1 && v1.is<std::string_view>())
    {
      // This string streams and C-style function stuff is a mess.
      // Should probably switch to Howard Hinnant's date.h or (later) C++20 in
      // the future.
      const std::string timeString = std::string(v1.get<std::string_view>().value());
      std::istringstream stream(timeString);
      std::tm t_old = {};
      stream >> std::get_time(&t_old, "%Y-%m-%d %H:%M");
      t_old.tm_isdst = -1; // unknown whether we have DST or not
      const std::time_t tt = std::mktime(&t_old);
      if (tt == static_cast<std::time_t>(-1))
        return std::chrono::time_point<std::chrono::system_clock>();
      return std::chrono::system_clock::from_time_t(tt);
    }
    // fall back to localtime_epoch - less precise / may be wrong timezone
    simdjson::dom::element v2;
    const auto e2 = location["localtime_epoch"].get(v2);
    if (!e2 && v2.is<int64_t>())
    {
      return std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2.get<int64_t>().value()));
    }
    else
    {
      std::cerr << "Error in SimdJsonWeatherstack::parseCurrentWeather(): location object does not have an integral localtime_epoch entry!" << std::endl;
      return std::chrono::time_point<std::chrono::system_clock>();
    }
  }
  else
  {
    std::cerr << "Error in SimdJsonWeatherstack::parseCurrentWeather(): JSON does not have a location object!" << std::endl;
    return std::chrono::time_point<std::chrono::system_clock>();
  }
}

bool SimdJsonWeatherstack::parseCurrentWeather(const std::string& json, Weather& weather)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto parseError = parser.parse(json).get(doc);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonWeatherstack::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(parseError) << std::endl;
    return false;
  }

  weather.setJson(json);

  simdjson::dom::element elem;
  auto error = doc["current"].get(elem);
  if (!error &&  elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const auto current = elem;
    simdjson::dom::element v2;
    // temperature
    current["temperature"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      weather.setTemperatureCelsius(static_cast<float>(v2.get<double>().value()));
      // Since there are no other values (Fahrenheit or Kelvin), we can just
      // calculate them on the fly.
      weather.setTemperatureFahrenheit(static_cast<float>(weather.temperatureCelsius() * 1.8 + 32.0));
      // Avoid values like 6.9999... ° F by rounding, if appropriate.
      const float fahrenheitRounded = NumericPrecision<float>::enforce(weather.temperatureFahrenheit());
      if (fahrenheitRounded != weather.temperatureFahrenheit())
      {
        weather.setTemperatureFahrenheit(fahrenheitRounded);
      }
      weather.setTemperatureKelvin(static_cast<float>(weather.temperatureCelsius() + 273.15));
    }
    // wind
    current["wind_degree"].tie(v2, error);
    if (!error && v2.is<int64_t>())
      weather.setWindDegrees(static_cast<int16_t>(v2.get<int64_t>().value()));
    current["wind_speed"].tie(v2, error);
    if (!error && v2.is<double>())
      weather.setWindSpeed(static_cast<float>(v2.get<double>().value() / 3.6));
    // humidity
    current["humidity"].tie(v2, error);
    if (!error && v2.is<int64_t>())
      weather.setHumidity(static_cast<int8_t>(v2.get<int64_t>().value()));
    // pressure
    current["pressure"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      weather.setPressure(static_cast<int16_t>(v2.get<double>().value()));
    }
    // rain or snow
    current["precip"].tie(v2, error);
    if (!error && v2.is<double>())
    {
      simdjson::dom::element weather_code;
      const auto errorCode = current["weather_code"].get(weather_code);
      const int code = (!errorCode && weather_code.is<int64_t>())
                       ? static_cast<int>(weather_code.get<int64_t>().value())
                       : 0;
      weatherstackPrecipitationDistinction(static_cast<float>(v2.get<double>().value()), weather, code);
    }

    // cloudiness
    current["cloudcover"].tie(v2, error);
    if (!error && v2.is<int64_t>())
      weather.setCloudiness(static_cast<int8_t>(v2.get<int64_t>().value()));
    // date of data update
    const auto dt = parseDateTime(doc);
    if (dt != std::chrono::time_point<std::chrono::system_clock>())
    {
      weather.setDataTime(dt);
      return true;
    }
    else
    {
      std::cerr << "Error in SimdJsonWeatherstack::parseCurrentWeather(): JSON does not have data time!" << std::endl;
      return false;
    }
  } // if current object

  // No current object - return false to indicate failure.
  return false;
}

} // namespace
