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

#include "Weatherbit.hpp"
#include <cmath>
#include <iostream>
#ifndef wic_no_json_parsing
#include "../json/NLohmannJsonWeatherbit.hpp"
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Curly.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

Weatherbit::Weatherbit(const std::string& key)
: m_apiKey(key)
{
}

void Weatherbit::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool Weatherbit::validLocation(const Location& location) const
{
  // Only latitude and longitude are required for a request.
  return location.hasCoordinates()
  // Name and country is another valid option.
    || (location.hasName() && location.hasCountryCode())
  // Postal code and country is also possible.
  // (Although the country is optional when using a postcode in Weatherbit, it
  //  is better to use the country code, too. Otherwise one may get unexpected
  //  results.)
    || (location.hasPostcode() && location.hasCountryCode());

  // Note: The other possibilities are not useful here.
  // (IP address should not be used, because it can change and it might be
  //  inaccurate. ICAO codes are only available to premium subscriptions.
  //  City IDs cannot easily be retrieved via the API, but only as JSON file
  //  download which is ca. 66 MB in size.)
}

bool Weatherbit::supportsDataType(const DataType data) const
{
  // Only current weather data and forecast can be retrieved, not both together.
  switch (data)
  {
    case DataType::Current:
    case DataType::Forecast:
         return true;
    case DataType::CurrentAndForecast:
    case DataType::none:
    default:
         return false;
  } // switch
}

#ifndef wic_no_json_parsing
bool Weatherbit::parseCurrentWeather(const std::string& json, Weather& weather) const
{
  return NLohmannJsonWeatherbit::parseCurrentWeather(json, weather);
}

bool Weatherbit::parseForecast(const std::string& json, Forecast& forecast) const
{
  return NLohmannJsonWeatherbit::parseForecast(json, forecast);
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string Weatherbit::toRequestString(const Location& location) const
{
  if (location.hasCoordinates())
    return "lat=" + std::to_string(location.latitude()) + std::string("&lon=")
         + std::to_string(location.longitude());
  if (location.hasName() && location.hasCountryCode())
    return std::string("city=") + location.name() + std::string("&country=")
         + location.countryCode();
  if (location.hasPostcode() && location.hasCountryCode())
    return std::string("postal_code=") + location.postcode()
         + std::string("&country=") + location.countryCode();

  // Note: The other possibilities are not useful here.
  // (IP address should not be used, because it can change and it might be
  //  inaccurate. ICAO codes are only available to premium subscriptions.
  //  City IDs cannot easily be retrieved via the API, but only as JSON file
  //  download which is ca. 66 MB in size.)

  // no required data set
  return std::string();
}

bool Weatherbit::currentWeather(const Location& location, Weather& weather)
{
  weather = Weather();
  if (m_apiKey.empty())
    return false;
  // https://api.weatherbit.io/v2.0/current?key=API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here
  const std::string url = "https://api.weatherbit.io/v2.0/current?key=" + m_apiKey
                        // Use the metric system, because we don't want any quarter pounder with cheese today.
                        + std::string("&units=M")
                        + "&" + toRequestString(location);
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
      std::cerr << "Error in Weatherbit::currentWeather(): Unexpected HTTP status code "
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

bool Weatherbit::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;
  // https://api.weatherbit.io/v2.0/forecast/3hourly?key=you-API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here
  const std::string url = "https://api.weatherbit.io/v2.0/forecast/3hourly?key=" + m_apiKey
                        // Use the metric system.
                        + std::string("&units=M")
                        + "&" + toRequestString(location);
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
      std::cerr << "Error in Weatherbit::forecastWeather(): Unexpected HTTP status code "
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

  // Parsing is done in another method.
  return parseForecast(response, forecast);
}

bool Weatherbit::currentAndForecastWeather(const Location& location, Weather& weather, Forecast& forecast)
{
  std::cerr << "Error: Getting both current weather and forecast data in a "
            << "single request is not supported by Weatherbit!" << std::endl;
  return false;
}
#endif // wic_no_network_requests

} // namespace
