/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2020, 2021, 2022, 2023, 2025  Dirk Stolle

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
#include <iostream>
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../json/SimdJsonWeatherbit.hpp"
#else
#include "../json/NLohmannJsonWeatherbit.hpp"
#endif
#endif // wic_no_json_parsing
#ifndef wic_no_network_requests
#include "../net/Request.hpp"
#endif // wic_no_network_requests
#include "../util/Strings.hpp"

namespace wic
{

Weatherbit::Weatherbit(const PlanWeatherbit plan, const std::string& key)
: m_apiKey(key),
  m_plan(plan)
{
}

void Weatherbit::setApiKey(const std::string& key)
{
  m_apiKey = key;
}

bool Weatherbit::needsApiKey()
{
  return true;
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
    default: // i. e. DataType::CurrentAndForecast and DataType::none
         return false;
  } // switch
}

#ifndef wic_no_json_parsing
bool Weatherbit::parseCurrentWeather(const std::string& json, Weather& weather) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonWeatherbit::parseCurrentWeather(json, weather);
#else
  return NLohmannJsonWeatherbit::parseCurrentWeather(json, weather);
#endif // __SIZEOF_INT128__
}

bool Weatherbit::parseForecast(const std::string& json, Forecast& forecast) const
{
#ifdef __SIZEOF_INT128__
  return SimdJsonWeatherbit::parseForecast(json, forecast);
#else
  return NLohmannJsonWeatherbit::parseForecast(json, forecast);
#endif
}
#endif // wic_no_json_parsing

#ifndef wic_no_network_requests
std::string Weatherbit::toRequestString(const Location& location)
{
  if (location.hasCoordinates())
    return "lat=" + floatToString(location.latitude()) + std::string("&lon=")
         + floatToString(location.longitude());
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
  weather.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "Weatherbit::currentWeather");
  if (!response.has_value())
    return false;

  // Parsing is done here.
  return parseCurrentWeather(response.value(), weather);
}

bool Weatherbit::forecastWeather(const Location& location, Forecast& forecast)
{
  forecast = Forecast();
  if (m_apiKey.empty())
    return false;
  // Daily forecast for 16 days (available for users on every plan of Weatherbit.io)
  // https://api.weatherbit.io/v2.0/forecast/daily?key=you-API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here
  // 3-hourly forecast for 120 hours / 5 days (only available for users on premium plans of Weatherbit.io, deprecated)
  // https://api.weatherbit.io/v2.0/forecast/3hourly?key=you-API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here
  // hourly forecast for up to 48 hours (or up to 120 for premium users)
  // https://api.weatherbit.io/v2.0/forecast/hourly?key=you-API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here
  std::string url;
  switch(m_plan)
  {
    case PlanWeatherbit::Free:
         url = "https://api.weatherbit.io/v2.0/forecast/daily?key=" + m_apiKey
               // Use the metric system.
               + std::string("&units=M")
               + "&" + toRequestString(location);
         break;
    case PlanWeatherbit::Standard:
    case PlanWeatherbit::Plus:
    case PlanWeatherbit::Business:
    default:
         url = "https://api.weatherbit.io/v2.0/forecast/hourly?key=" + m_apiKey
               // Use the metric system.
               + std::string("&units=M")
               + "&" + toRequestString(location);
         break;
  }
  forecast.setRequestTime(std::chrono::system_clock::now());
  const auto response = Request::get(url, "Weatherbit::forecastWeather");
  if (!response.has_value())
    return false;

  // Parsing is done in another method.
  return parseForecast(response.value(), forecast);
}

bool Weatherbit::currentAndForecastWeather([[maybe_unused]] const Location& location,
                                           [[maybe_unused]] Weather& weather,
                                           [[maybe_unused]] Forecast& forecast)
{
  std::cerr << "Error: Getting both current weather and forecast data in a "
            << "single request is not supported by Weatherbit!" << std::endl;
  return false;
}
#endif // wic_no_network_requests

} // namespace
