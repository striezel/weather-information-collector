/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#include "SimdJsonDarkSky.hpp"
#include <iostream>
#include "../util/NumericPrecision.hpp"

namespace wic
{

bool SimdJsonDarkSky::parseCurrentWeather(const std::string& json, Weather& weather)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonDarkSky::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(error) << std::endl;
    return false;
  }

  weather.setJson(json);

  // Current weather data is located in the currently object below the root.
  simdjson::dom::element currently;
  simdjson::error_code e;
  doc["currently"].tie(currently, e);
  if (e)
  {
    std::cerr << "Error in SimdJsonDarkSky::parseCurrentWeather(): No JSON element \"currently\" found!" << std::endl;
    return false;
  }
  return parseSingleWeatherItem(currently, weather);
}

bool SimdJsonDarkSky::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonDarkSky::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(error) << std::endl;
    return false;
  }

  forecast.setJson(json);

  simdjson::dom::element hourly;
  const auto e = doc["hourly"].get(hourly);
  if (e || hourly.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error in SimdJsonDarkSky::parseForecast(): The element \"hourly\" is missing or is not an object!" << std::endl;
    return false;
  }

  simdjson::dom::element hourlyData;
  const auto e2 = hourly["data"].get(hourlyData);
  if (e2 || hourlyData.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonDarkSky::parseForecast(): The element \"data\" is missing or is not an array!" << std::endl;
    return false;
  }

  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type& val : hourlyData)
  {
    Weather w;
    if (!parseSingleWeatherItem(val, w))
    {
      std::cerr << "Error in SimdJsonDarkSky::parseForecast(): Parsing of element in data array failed!" << std::endl;
      return false;
    }
    data.push_back(w);
  } // for (range-based)
  // Set data to parsed data ...
  forecast.setData(data);
  // And we are done here.
  return true;
}

bool SimdJsonDarkSky::parseSingleWeatherItem(const value_type& dataPoint, Weather& weather)
{
  if (dataPoint.type() != simdjson::dom::element_type::OBJECT)
    return false;

  simdjson::dom::element elem;
  auto error = dataPoint["time"].get(elem);
  if (!error && elem.is<int64_t>())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(elem.get<int64_t>().value()));
    weather.setDataTime(dt);
  }
  // temperature (in °C)
  dataPoint["temperature"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    weather.setTemperatureCelsius(static_cast<float>(elem.get<double>().value()));
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
  // relative humidity, [0;1]
  dataPoint["humidity"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    const long int humidity = std::lround(elem.get<double>().value() * 100);
    weather.setHumidity(static_cast<int8_t>(humidity));
  }
  // rain or snow (mm/m² in an hour)
  dataPoint["precipIntensity"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    const float amount = static_cast<float>(elem.get<double>().value());
    simdjson::dom::element findType;
    const auto typeError = dataPoint["precipType"].get(findType);
    if (!typeError && findType.is<std::string_view>())
    {
      const std::string_view typeStr = findType.get<std::string_view>().value();
      if (typeStr == "rain")
      {
        weather.setRain(amount);
        weather.setSnow(0.0);
      }
      else if (typeStr == "snow")
      {
        weather.setRain(0.0);
        weather.setSnow(amount);
      }
      else if (typeStr == "sleet")
      {
        // Sleet could be described as mixture of snow and rain.
        weather.setRain(amount);
        weather.setSnow(amount);
      }
      else
      {
        std::cerr << "Error in SimdJsonDarkSky::parseCurrentWeather(): Unknown precipType "
                  << typeStr << "!" << std::endl;
        return false;
      }
    }
    // precipType may not be present, if precipIntensity is zero.
    else if (amount == 0.0)
    {
      weather.setRain(0.0);
      weather.setSnow(0.0);
    }
    else
    {
      std::cerr << "Error in SimdJsonDarkSky::parseCurrentWeather(): Missing precipType!"
                << std::endl;
      return false;
    }
  } // if precipIntensity
  // pressure [hPa]
  dataPoint["pressure"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    const int16_t press = static_cast<int16_t>(std::lround(elem.get<double>().value()));
    weather.setPressure(press);
  }
  // wind speed [m/s]
  dataPoint["windSpeed"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    weather.setWindSpeed(static_cast<float>(elem.get<double>().value()));
  }
  // wind windBearing [°], with 0=N,90=E,180=S,270=W
  dataPoint["windBearing"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    const int16_t degrees = static_cast<int16_t>(std::lround(elem.get<double>().value()));
    weather.setWindDegrees(degrees);
  }
  // cloud cover, [0;1]
  dataPoint["cloudCover"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    long int cloudCover = std::min(101L, std::lround(elem.get<double>().value() * 100));
    cloudCover = std::max(-1L, cloudCover);
    weather.setCloudiness(static_cast<int8_t>(cloudCover));
  }
  return weather.hasDataTime() && weather.hasTemperatureCelsius() && weather.hasHumidity();
}

} // namespace
