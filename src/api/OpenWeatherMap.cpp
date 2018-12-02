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

#include "OpenWeatherMap.hpp"
#include <iostream>
#include <jsoncpp/json/reader.h>
#include "../net/Curly.hpp"
#include "../util/Strings.hpp"

namespace wic
{

OpenWeatherMap::OpenWeatherMap(const std::string& key)
: m_apiKey(key)
{
}

void OpenWeatherMap::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool OpenWeatherMap::validLocation(const Location& location) const
{
  return (location.hasId() || location.hasCoordinates()
      || location.hasName() || location.hasPostcode());
}

bool OpenWeatherMap::supportsDataType(const DataType data) const
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

std::string OpenWeatherMap::toRequestString(const Location& location) const
{
  if (location.hasId())
    return std::string("id=") + intToString(location.id());
  if (location.hasCoordinates())
    return std::string("lat=") + floatToString(location.latitude())
         + std::string("&lon=") + floatToString(location.longitude());
  if (location.hasName())
    return std::string("q=") + location.name();
  if (location.hasPostcode())
    return std::string("zip=") + location.postcode();
  // no required data set
  return std::string();
}

bool OpenWeatherMap::parseSingleWeatherItem(const Json::Value& value, Weather& weather) const
{
  if (value.empty())
    return false;
  Json::Value val = value["main"];
  bool foundValidParts = false;
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["temp"];
    if (!v2.empty() && v2.isDouble())
    {
      weather.setTemperatureKelvin(v2.asFloat());
      weather.setTemperatureCelsius(weather.temperatureKelvin() - 273.15);
      weather.setTemperatureFahrenheit(weather.temperatureCelsius() * 1.8 + 32.0f);
    }
    v2 = val["pressure"];
    if (!v2.empty() && v2.isNumeric())
      weather.setPressure(static_cast<int16_t>(v2.asFloat()));
    v2 = val["humidity"];
    if (!v2.empty() && v2.isIntegral())
      weather.setHumidity(v2.asInt());
    foundValidParts = true;;
  } // if main object
  val = value["wind"];
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["speed"];
    if (!v2.empty() && v2.isDouble())
      weather.setWindSpeed(v2.asFloat());
    v2 = val["deg"];
    if (!v2.empty() && v2.isNumeric())
      weather.setWindDegrees(static_cast<int16_t>(v2.asFloat()));
  } // if wind object
  val = value["clouds"];
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["all"];
    if (!v2.empty() && v2.isIntegral())
      weather.setCloudiness(v2.asInt());
  } // if clouds object
  val = value["rain"];
  if (!val.empty() && val.isObject())
  {
    Json::Value v2 = val["3h"];
    if (!v2.empty() && (v2.isDouble() || v2.isIntegral()))
      weather.setRain(v2.asFloat());
  } // if rain object
  val = value["dt"];
  if (!val.empty() && val.isIntegral())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(val.asInt()));
    weather.setDataTime(dt);
  } // if dt
  return foundValidParts;
}

bool OpenWeatherMap::parseCurrentWeather(const std::string& json, Weather& weather) const
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in OpenWeatherMap::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(root, weather);
}

bool OpenWeatherMap::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  const std::string url = "https://api.openweathermap.org/data/2.5/weather?appid="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    weather.setRequestTime(std::chrono::system_clock::now());
    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in OpenWeatherMap::currentWeather(): Unexpected HTTP status code "
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

  // Parsing is done here.
  return parseCurrentWeather(response, weather);
}

bool OpenWeatherMap::parseForecast(const std::string& json, Forecast& forecast) const
{
  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(json, root, false);
  if (!success)
  {
    std::cerr << "Error in OpenWeatherMap::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (root.empty())
    return false;

  const Json::Value list = root["list"];
  if (list.empty() || !list.isArray())
  {
    std::cerr << "Error in OpenWeatherMap::parseForecast(): list is either empty or not an array!" << std::endl;
    return false;
  }
  forecast.setData({ });
  auto data = forecast.data();
  for (const Json::Value val : list)
  {
    Weather w;
    if (parseSingleWeatherItem(val, w))
    {
      data.push_back(w);
    }
    else
    {
      std::cerr << "Error in OpenWeatherMap::parseForecast(): Parsing single item failed!" << std::endl;
      return false;
    }
  } // for (range-based)
  const auto val = root["cnt"];
  if (val.empty() || !val.isIntegral())
  {
    std::cerr << "Error in OpenWeatherMap::parseForecast(): cnt is empty or not an integer!" << std::endl;
    return false;
  }
  const decltype(data.size()) cnt = val.asUInt();
  // Number of data items should be the number given in "cnt".
  if (data.size() != cnt)
  {
    std::cerr << "Error in OpenWeatherMap::parseForecast(): Expected " << cnt
              << " items, but only " << data.size() << " items were found!" << std::endl;
    return false;
  }
  forecast.setData(data);
  return true;
}

bool OpenWeatherMap::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty() || location.empty())
    return false;
  const std::string url = "https://api.openweathermap.org/data/2.5/forecast?appid="
                        + m_apiKey + "&" + toRequestString(location);
  std::string response;
  {
    Curly curly;
    curly.setURL(url);

    forecast.setRequestTime(std::chrono::system_clock::now());
    if (!curly.perform(response))
    {
      return false;
    }
    if (curly.getResponseCode() != 200)
    {
      std::cerr << "Error in OpenWeatherMap::forecastWeather(): Unexpected HTTP status code "
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

  // Parsing is done here.
  return parseForecast(response, forecast);
}

bool OpenWeatherMap::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  std::cerr << "Error: Getting both current weather and forecast data in a "
            << "single request is not supported by OpenWeatherMap!" << std::endl;
  return false;
}

#ifdef wic_task_creator
std::string urlEncode(const std::string& str)
{
  static const char hexDigits[16] = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  std::string encoded;
  for (auto it = str.begin(); it != str.end(); ++it)
  {
    const char c = *it;
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z') || (c == '-') || (c == '_')
      || (c == '.') || (c == '~'))
    {
      encoded.push_back(c);
    }
    else
    {
      // d
      encoded.push_back('%');
      encoded.push_back(hexDigits[c / 16]);
      encoded.push_back(hexDigits[c % 16]);
    }
  } // for
  return encoded;
}

bool OpenWeatherMap::findLocation(const std::string& name, std::vector<std::pair<LocationWithCountry, Weather> >& locations) const
{
  if (m_apiKey.empty() || name.empty())
    return false;

  const std::string url = "https://api.openweathermap.org/data/2.5/find?q=" + urlEncode(name) + "&appid=" + m_apiKey;
  Curly curly;
  curly.setURL(url);
  std::string response;
  if (!curly.perform(response))
  {
    return false;
  }
  if (curly.getResponseCode() != 200)
  {
    std::cerr << "Error in OpenWeatherMap::findLocation(): Unexpected HTTP status code "
              << curly.getResponseCode() << "!" << std::endl;
    const auto & rh = curly.responseHeaders();
    std::cerr << "HTTP response headers (" << rh.size() << "):" << std::endl;
    for (const auto & s : rh)
    {
      std::cerr << "    " << s << std::endl;
    }
    return false;
  }

  Json::Value root; // will contain the root value after parsing.
  Json::Reader jsonReader;
  const bool success = jsonReader.parse(response, root, false);
  if (!success)
  {
    std::cerr << "Error in OpenWeatherMap::findLocation(): Unable to parse JSON data!" << std::endl;
    return false;
  }
  Json::Value count = root["count"];
  if (count.empty() || !count.isIntegral())
  {
    std::cerr << "Error in OpenWeatherMap::findLocation(): JSON element for count is missing!" << std::endl;
    return false;
  }
  locations.clear();
  if (count.asInt() == 0)
    return true;

  const Json::Value list = root["list"];
  if (list.empty() || !list.isArray())
  {
    std::cerr << "Error in OpenWeatherMap::findLocation(): JSON list element is missing!" << std::endl;
    return false;
  }

  for (const Json::Value elem : list)
  {
    LocationWithCountry loc;
    Json::Value val = elem["id"];
    if (!val.empty() && val.isUInt())
      loc.setId(val.asUInt());
    val = elem["name"];
    if (!val.empty() && val.isString())
      loc.setName(val.asString());
    const Json::Value coord = elem["coord"];
    if (!coord.empty() && coord.isObject())
    {
      val = coord["lat"];
      Json::Value lon = coord["lon"];
      if (!val.empty() && !lon.empty() && val.isNumeric() && lon.isNumeric())
      {
        loc.setCoordinates(val.asFloat(), lon.asFloat());
      }
    } // coord
    if (loc.empty())
    {
      std::cerr << "Error in OpenWeatherMap::findLocation(): Location data is empty!" << std::endl;
      return false;
    }
    val = elem["sys"];
    if (!val.empty() && val.isObject())
    {
      val = val["country"];
      if (!val.empty() && val.isString())
        loc.setCountry(val.asString());
    }
    Weather w;
    if (!parseSingleWeatherItem(elem, w))
    {
      std::cerr << "Error in OpenWeatherMap::findLocation(): Weather data for location is missing!" << std::endl;
      return false;
    }
    if (loc.empty())
    {
      std::cerr << "Error in OpenWeatherMap::findLocation(): Location data is empty!" << std::endl;
      return false;
    }
    // add element to result
    locations.push_back(std::make_pair(loc, w));
  } // for

  return true;
}
#endif

} // namespace
