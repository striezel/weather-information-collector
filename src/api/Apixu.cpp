/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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

#include "Apixu.hpp"
#include <iostream>
#include <jsoncpp/json/reader.h>
#include "../net/Curly.hpp"
#include "../util/Strings.hpp"

namespace wic
{

Apixu::Apixu(const std::string& key)
: m_apiKey(key)
{
}

void Apixu::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool Apixu::validLocation(const Location& location) const
{
  return (location.hasCoordinates() || location.hasName()
       || location.hasPostcode());
}

bool Apixu::supportsDataType(const DataType data) const
{
  // Only current weather data and forecast can be retrieved, not both together.
  switch (data)
  {
    case DataType::Current:
    case DataType::Forecast:
         return true;
    case DataType::CurrentAndForecast:
    default:
         return false;
  }
}

std::string Apixu::toRequestString(const Location& location) const
{
  if (location.hasCoordinates())
    return std::string("q=") + floatToString(location.latitude())
         + std::string(",") + floatToString(location.longitude());
  if (location.hasName())
    return std::string("q=") + location.name();
  if (location.hasPostcode())
    return std::string("q=") + location.postcode();
  // no required data set
  return std::string();
}

bool Apixu::parseCurrentWeather(const std::string& json, Weather& weather) const
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in Apixu::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);
  weather.setRequestTime(std::chrono::system_clock::now());

  if (root.empty())
    return false;
  Json::Value val = root["current"];
  if (!val.empty() && val.isObject())
  {
    // temperature
    Json::Value v2 = val["temp_c"];
    if (!v2.empty())
    {
      if (v2.isDouble())
        weather.setTemperatureCelsius(v2.asFloat());
      else if (v2.isIntegral())
        weather.setTemperatureCelsius(v2.asInt());
    }
    v2 = val["temp_f"];
    if (!v2.empty() && v2.isDouble())
      weather.setTemperatureFahrenheit(v2.asFloat());
    // wind
    v2 = val["wind_degree"];
    if (!v2.empty() && v2.isIntegral())
      weather.setWindDegrees(v2.asInt());
    v2 = val["wind_kph"];
    if (!v2.empty() && v2.isDouble())
      weather.setWindSpeed(v2.asFloat() / 3.6);
    else
    {
      v2 = val["wind_mph"];
      if (!v2.empty() && v2.isDouble())
        weather.setWindSpeed(v2.asFloat() * 1.609344 / 3.6);
    }
    // humidity
    v2 = val["humidity"];
    if (!v2.empty() && v2.isIntegral())
      weather.setHumidity(v2.asInt());
    // rain
    v2 = val["precip_mm"];
    if (!v2.empty() && (v2.isDouble() || v2.isIntegral()))
      weather.setRain(v2.asFloat());
    // pressure
    v2 = val["pressure_mb"];
    if (!v2.empty())
    {
      if (v2.isDouble())
        weather.setPressure(v2.asDouble());
      else if (v2.isIntegral())
        weather.setPressure(v2.asInt());
    }
    // cloudiness
    v2 = val["cloud"];
    if (!v2.empty() && v2.isIntegral())
      weather.setCloudiness(v2.asInt());
    // date of data update
    v2 = val["last_updated_epoch"];
    if (!v2.empty() && v2.isIntegral())
    {
      const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(v2.asInt()));
      weather.setDataTime(dt);
    }
    return true;
  } // if current object

  // No current object - return false to indicate failure.
  return false;
}

bool Apixu::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.apixu.com/v1/current.json?key="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in Apixu::currentWeather(): Unexpected HTTP status code "
                << curly.getResponseCode() << "!" << std::endl;
      const auto & rh = curly.responseHeaders();
      std::cerr << "HTTP response headers (" << rh.size() << "):" << std::endl;
      for (const auto & s : rh)
      {
        std::cerr << "    " << s << std::endl;
      }
      return false;
    }
  } // scope of curly

  return parseCurrentWeather(response, weather);
}

bool Apixu::parseForecast(const std::string& json, Forecast& forecast) const
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in Apixu::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const Json::Value jsForecast = root["forecast"];
  if (jsForecast.empty())
    return false;
  const Json::Value forecastday = jsForecast["forecastday"];
  // forecastday must be a non-empty array.
  if (forecastday.empty() || !forecastday.isArray())
    return false;
  forecast.setData({ });
  auto data = forecast.data();
  for (const Json::Value& value : forecastday)
  {
    Weather w_min;
    const Json::Value date_epoch = value["date_epoch"];
    if (date_epoch.empty() || !date_epoch.isIntegral())
      return false;
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch.asInt()));
    w_min.setDataTime(dt);
    const Json::Value day = value["day"];
    // day must be a non-empty object.
    if (day.empty() || !day.isObject())
      return false;
    Json::Value val = day["mintemp_c"];
    if (!val.empty() && val.isNumeric())
    {
      w_min.setTemperatureCelsius(val.asFloat());
    }
    val = day["mintemp_f"];
    if (!val.empty() && val.isNumeric())
    {
      w_min.setTemperatureFahrenheit(val.asFloat());
    }
    const Json::Value avghumidity = day["avghumidity"];
    if (!avghumidity.empty() && avghumidity.isNumeric())
      w_min.setHumidity(avghumidity.asInt());
    const Json::Value totalprecip_mm = day["totalprecip_mm"];
    if (!totalprecip_mm.empty() && totalprecip_mm.isNumeric())
      w_min.setRain(totalprecip_mm.asFloat());
    data.push_back(w_min);

    Weather w_max;
    w_max.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(date_epoch.asInt() + 3600 * 12)));
    val = day["maxtemp_c"];
    if (!val.empty() && val.isNumeric())
    {
      w_max.setTemperatureCelsius(val.asFloat());
    }
    val = day["maxtemp_f"];
    if (!val.empty() && val.isNumeric())
    {
      w_max.setTemperatureFahrenheit(val.asFloat());
    }
    if (!avghumidity.empty() && avghumidity.isNumeric())
      w_max.setHumidity(avghumidity.asInt());
    if (!totalprecip_mm.empty() && totalprecip_mm.isNumeric())
      w_max.setRain(totalprecip_mm.asFloat());
    data.push_back(w_max);
  } // for (range-based)

  forecast.setData(data);
  return true;
}

bool Apixu::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;

  const std::string url = "https://api.apixu.com/v1/forecast.json?days=7&key="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in Apixu::forecastWeather(): Unexpected HTTP status code "
                << curly.getResponseCode() << "!" << std::endl;
      const auto & rh = curly.responseHeaders();
      std::cerr << "HTTP response headers (" << rh.size() << "):" << std::endl;
      for (const auto & s : rh)
      {
        std::cerr << "    " << s << std::endl;
      }
      return false;
    }
  } // scope of curly
  // TODO: implement it!
  return false;
}

} // namespace
