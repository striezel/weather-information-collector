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

#include "OpenMeteo.hpp"
#include <iostream>
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonOpenMeteo.hpp"
#else
#include "../json/NLohmannJsonOpenMeteo.hpp"
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Request.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

void OpenMeteo::setApiKey([[maybe_unused]] const std::string& key)
{
  // Open-Meteo uses no API keys, but an implementation is required by the base
  // class / interface API.
}

bool OpenMeteo::validLocation(const Location& location) const
{
  // Only latitude and longitude are required for a request.
  return location.hasCoordinates();
}

bool OpenMeteo::supportsDataType(const DataType data) const
{
  // All three data types (DataType::Current, DataType::Forecast, and
  // DataType::CurrentAndForecast) are supported.
  return (data != DataType::none);
}

#ifndef wic_no_json_parsing
bool OpenMeteo::parseCurrentWeather(const std::string& json, Weather& weather) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonOpenMeteo::parseCurrentWeather(json, weather);
#else
  return NLohmannJsonOpenMeteo::parseCurrentWeather(json, weather);
#endif // __SIZEOF_INT128__
}

bool OpenMeteo::parseForecast(const std::string& json, Forecast& forecast) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonOpenMeteo::parseForecast(json, forecast);
#else
  return NLohmannJsonOpenMeteo::parseForecast(json, forecast);
#endif // __SIZEOF_INT128__
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string OpenMeteo::toRequestString(const Location& location)
{
  if (location.hasCoordinates())
    return "latitude=" + floatToString(location.latitude()) + "&longitude="
         + floatToString(location.longitude());
  // no required data set
  return std::string();
}

bool OpenMeteo::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  const std::string url = "https://api.open-meteo.com/v1/forecast?"
                        + toRequestString(location) + "&current_weather=true&windspeed_unit=ms&timezone=auto";
  weather.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "OpenMeteo::currentWeather");
  if (!response.has_value())
    return false;

  // Parsing is done here.
  return parseCurrentWeather(response.value(), weather);
}

bool OpenMeteo::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  const std::string url = "https://api.open-meteo.com/v1/forecast?"
                        + toRequestString(location)
                        + "&hourly=temperature_2m,relativehumidity_2m,precipitation,rain,showers,snowfall,pressure_msl,surface_pressure,cloudcover,windspeed_10m,winddirection_10m&windspeed_unit=ms&timezone=auto";
  forecast.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "OpenMeteo::forecastWeather");
  if (!response.has_value())
    return false;

  // Parsing is done in another method.
  return parseForecast(response.value(), forecast);
}

bool OpenMeteo::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  weather = Weather();
  forecast = Forecast();
  const std::string url = "https://api.open-meteo.com/v1/forecast?"
                        + toRequestString(location)
                        + "&current_weather=true&hourly=temperature_2m,relativehumidity_2m,precipitation,rain,showers,snowfall,pressure_msl,surface_pressure,cloudcover,windspeed_10m,winddirection_10m&windspeed_unit=ms&timezone=auto";
  forecast.setRequestTime(std::chrono::system_clock::now());
  weather.setRequestTime(forecast.requestTime());
  const auto response = Request::get(url, "OpenMeteo::currentAndForecastWeather");
  if (!response.has_value())
    return false;

  // Parse current weather and forecast with the already existing functions.
  if (!parseCurrentWeather(response.value(), weather))
    return false;
  return parseForecast(response.value(), forecast);
}
#endif // wic_no_network_requests

} // namespace
