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

#include "SimdJsonWeatherbit.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool SimdJsonWeatherbit::parseSingleWeatherItem(const value_type& value, Weather& weather)
{
  if (value.type() != simdjson::dom::element_type::OBJECT)
  {
    std::cerr << "Error: Type of item is " << value.type() << ", but it should be object!\n";
    return false;
  }
  auto [elem, error] = value["temp"];
  if (!error && elem.is<double>())
  {
    const float celsiusRaw = elem.get<double>().value();
    weather.setTemperatureCelsius(celsiusRaw);
    // Avoid values like 5.9999... °C by rounding, if appropriate.
    const float celsiusRounded = std::round(weather.temperatureCelsius());
    if (std::fabs(celsiusRounded - weather.temperatureCelsius()) < 0.005)
    {
      weather.setTemperatureCelsius(celsiusRounded);
    }
    weather.setTemperatureKelvin(celsiusRaw + 273.15);
    // Avoid values like 280.9999... K by rounding, if appropriate.
    const float kelvinRounded = std::round(weather.temperatureKelvin());
    if (std::fabs(kelvinRounded - weather.temperatureKelvin()) < 0.005)
    {
      weather.setTemperatureKelvin(kelvinRounded);
    }
    weather.setTemperatureFahrenheit(celsiusRaw * 1.8 + 32.0f);
    // Avoid values like 6.9999... ° F by rounding, if appropriate.
    const float fahrenheitRounded = std::round(weather.temperatureFahrenheit());
    if (std::fabs(fahrenheitRounded - weather.temperatureFahrenheit()) < 0.005)
    {
      weather.setTemperatureFahrenheit(fahrenheitRounded);
    }
  }
  value["rh"].tie(elem, error);
  if (!error && elem.is<uint64_t>())
    weather.setHumidity(elem.get<int64_t>().value());
  value["precip"].tie(elem, error);
  if (!error && elem.is<double>())
    weather.setRain(elem.get<double>().value());
  value["snow"].tie(elem, error);
  if (!error && elem.is<double>())
    weather.setSnow(elem.get<double>().value());
  value["pres"].tie(elem, error);
  if (!error && elem.is<double>())
    weather.setPressure(static_cast<int16_t>(elem.get<double>().value()));
  value["wind_spd"].tie(elem, error);
  if (!error && elem.is<double>())
    weather.setWindSpeed(elem.get<double>().value());
  value["wind_dir"].tie(elem, error);
  if (!error && elem.is<uint64_t>())
    weather.setWindDegrees(static_cast<int16_t>(elem.get<int64_t>().value()));
  value["clouds"].tie(elem, error);
  if (!error && elem.is<uint64_t>())
      weather.setCloudiness(elem.get<int64_t>());

  value["ts"].tie(elem, error);
  if (!error && elem.is<double>())
  {
    // I'm not sure whether this really works as expected.
    // Weatherbit's documentation says the timestamp is UTC, but tests show that
    // this may not be the case.
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(static_cast<int64_t>(elem.get<double>().value())));
    weather.setDataTime(dt);
  } // if ts
  return weather.hasDataTime();
}

bool SimdJsonWeatherbit::parseCurrentWeather(const std::string& json, Weather& weather)
{
  simdjson::dom::parser parser;
  const auto [doc, parseError] = parser.parse(json);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonWeatherbit::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(parseError) << std::endl;
    return false;
  }

  const auto [data, errorData] = doc["data"];
  if (errorData || data.type() != simdjson::dom::element_type::ARRAY /*|| data.size() != 1*/)
  {
    std::cerr << "Error in SimdJsonWeatherbit::parseCurrentWeather(): JSON "
              << "does not contain a data element or data element is not an "
              << "array containing a single element!" << std::endl;
    return false;
  }
  const auto [count, errorCount] = doc["count"];
  if (errorCount || !count.is<uint64_t>())
  {
    std::cerr << "Error in SimdJsonWeatherbit::parseCurrentWeather(): JSON does not contain a count element or the element is not a valid number!" << std::endl;
    return false;
  }
  if (count.get<uint64_t>().value() != 1)
  {
    std::cerr << "Error in SimdJsonWeatherbit::parseCurrentWeather(): Expected count to be 1, but the actual count is "
              << count.get<uint64_t>().value() << "!" << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(data.at(0), weather);
}

bool SimdJsonWeatherbit::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  const auto [doc, parseError] = parser.parse(json);
  if (parseError)
  {
    std::cerr << "Error in SimdJsonWeatherbit::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(parseError) << std::endl;
    return false;
  }

  forecast.setJson(json);

  const auto [dataJson, errorData] = doc["data"];
  if (errorData || dataJson.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonWeatherbit::parseForecast(): data is not an array!" << std::endl;
    return false;
  }
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type val : dataJson)
  {
    Weather w;
    if (parseSingleWeatherItem(val, w))
    {
      data.push_back(w);
    }
    else
    {
      std::cerr << "Error in SimdJsonWeatherbit::parseForecast(): Parsing single item failed!" << std::endl;
      return false;
    }
  } // for (range-based)
  forecast.setData(data);
  return true;
}

} // namespace
