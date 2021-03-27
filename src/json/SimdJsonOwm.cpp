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

#include "SimdJsonOwm.hpp"
#include <cmath>
#include <iostream>

namespace wic
{

bool SimdJsonOwm::parseSingleWeatherItem(const value_type& value, Weather& weather)
{
  bool foundValidParts = false;
  auto [elem, error] = value["main"];
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type main = value["main"];
    main["temp"].tie(elem, error);
    if (!error && elem.is<double>())
    {
      const float kelvinRaw = elem.get<double>().value();
      weather.setTemperatureKelvin(kelvinRaw);
      // Avoid values like 280.9999... K by rounding, if appropriate.
      const float kelvinRounded = std::round(weather.temperatureKelvin());
      if (std::fabs(kelvinRounded - weather.temperatureKelvin()) < 0.005)
      {
        weather.setTemperatureKelvin(kelvinRounded);
      }
      weather.setTemperatureCelsius(kelvinRaw - 273.15);
      // Avoid values like 6.9999... ° C by rounding, if appropriate.
      const float celsiusRounded = std::round(weather.temperatureCelsius());
      if (std::fabs(celsiusRounded - weather.temperatureCelsius()) < 0.005)
      {
        weather.setTemperatureCelsius(celsiusRounded);
      }
      weather.setTemperatureFahrenheit((kelvinRaw - 273.15) * 1.8 + 32.0f);
      // Avoid values like 6.9999... ° F by rounding, if appropriate.
      const float fahrenheitRounded = std::round(weather.temperatureFahrenheit());
      if (std::fabs(fahrenheitRounded - weather.temperatureFahrenheit()) < 0.005)
      {
        weather.setTemperatureFahrenheit(fahrenheitRounded);
      }
    }
    main["pressure"].tie(elem, error);
    if (!error && elem.is<double>())
      weather.setPressure(static_cast<int16_t>(elem.get<double>()));
    main["humidity"].tie(elem, error);
    if (!error && elem.is<uint64_t>())
      weather.setHumidity(elem.get<uint64_t>());
    foundValidParts = true;
  } // if main object
  value["wind"].tie(elem, error);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type wind = elem;
    wind["speed"].tie(elem, error);
    if (!error && elem.is<double>())
      weather.setWindSpeed(elem.get<double>().value());
    wind["deg"].tie(elem, error);
    if (!error && elem.is<double>())
      weather.setWindDegrees(static_cast<int16_t>(elem.get<double>().value()));
  } // if wind object
  value["clouds"].tie(elem, error);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type clouds = elem;
    clouds["all"].tie(elem, error);
    if (!error && elem.is<int64_t>())
      weather.setCloudiness(elem.get<int64_t>().value());
  } // if clouds object
  value["rain"].tie(elem, error);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type rain = elem;
    rain["3h"].tie(elem, error);
    if (!error)
    {
      if (elem.is<double>())
        weather.setRain(elem.get<double>().value());
    }
    else
    {
      // Empty rain object means zero rain.
      weather.setRain(0.0f);
    }
  } // if rain object
  value["snow"].tie(elem, error);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type snow = elem;
    snow["3h"].tie(elem, error);
    if (!error)
    {
      if (elem.is<double>())
        weather.setSnow(elem.get<double>().value());
    }
    else
    {
      // Empty snow object means zero snow.
      weather.setSnow(0.0f);
    }
  } // if rain object
  value["dt"].tie(elem, error);
  if (!error && elem.is<int64_t>())
  {
    const auto dt = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(elem.get<int64_t>().value()));
    weather.setDataTime(dt);
  } // if dt
  return foundValidParts;
}

bool SimdJsonOwm::parseCurrentWeather(const std::string& json, Weather& weather)
{
  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(json);
  if (error)
  {
    std::cerr << "Error in SimdJsonOwm::parseCurrentWeather(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(doc, weather);
}

bool SimdJsonOwm::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(json);
  if (error)
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  forecast.setJson(json);

  const auto [list, e2] = doc["list"];
  if (e2 || list.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): list is either empty or not an array!" << std::endl;
    return false;
  }
  forecast.setData({ });
  auto data = forecast.data();
  for (const value_type val : list)
  {
    Weather w;
    if (parseSingleWeatherItem(val, w))
    {
      data.push_back(w);
    }
    else
    {
      std::cerr << "Error in SimdJsonOwm::parseForecast(): Parsing single item failed!" << std::endl;
      return false;
    }
  } // for (range-based)
  const auto [cnt, e3] = doc["cnt"];
  if (e3 || !cnt.is<uint64_t>())
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): cnt is empty or not an integer!" << std::endl;
    return false;
  }
  const decltype(data.size()) cntValue = cnt.get<uint64_t>();
  // Number of data items should be the number given in "cnt".
  if (data.size() != cntValue)
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): Expected " << cntValue
              << " items, but " << data.size() << " items were found!" << std::endl;
    return false;
  }
  forecast.setData(data);
  return true;
}

#ifdef wic_owm_find_location
bool SimdJsonOwm::parseLocations(const std::string& json, std::vector<std::pair<Location, Weather> >& locations)
{
  simdjson::dom::parser parser;
  const auto [doc, error] = parser.parse(json);
  if (error)
  {
    std::cerr << "Error in SimdJsonOwm::parseLocations(): Unable to parse JSON data!" << std::endl;
    return false;
  }

  const auto [count, e2] = doc["count"];
  if (e2 || !count.is<int64_t>())
  {
    std::cerr << "Error in SimdJsonOwm::parseLocations(): JSON element for count is missing!" << std::endl;
    return false;
  }
  locations.clear();
  if (count.get<int64_t>().value() == 0)
    return true;

  const auto [list, e3] = doc["list"];
  if (e3 || list.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonOwm::parseLocations(): JSON list element is missing!" << std::endl;
    return false;
  }

  for (const value_type elem : list)
  {
    Location loc;
    auto [val, err] = elem["id"];
    if (!err && val.is<uint64_t>())
      loc.setOwmId(val.get<uint64_t>());
    elem["name"].tie(val, err);
    if (!err && val.is<std::string_view>())
      loc.setName(std::string(val.get<std::string_view>().value()));
    const auto [coord, errorCoord] = elem["coord"];
    if (!errorCoord && coord.type() == simdjson::dom::element_type::OBJECT)
    {
      const auto [lat, errorLat] = coord["lat"];
      const auto [lon, errorLon] = coord["lon"];
      if (!errorLat && !errorLon && lat.is<double>() && lon.is<double>())
      {
        loc.setCoordinates(lat.get<double>().value(), lon.get<double>().value());
      }
    } // coord
    if (loc.empty())
    {
      std::cerr << "Error in SimdJsonOwm::parseLocations(): Location data is empty!" << std::endl;
      return false;
    }
    elem["sys"].tie(val, err);
    if (!err && val.type() == simdjson::dom::element_type::OBJECT)
    {
      const auto [country, errCountry] = val["country"];
      if (!errCountry && country.is<std::string_view>())
        loc.setCountryCode(std::string(country.get<std::string_view>().value()));
    }
    Weather w;
    if (!SimdJsonOwm::parseSingleWeatherItem(elem, w))
    {
      std::cerr << "Error in SimdJsonOwm::parseLocations(): Weather data for location is missing!" << std::endl;
      return false;
    }
    if (loc.empty())
    {
      std::cerr << "Error in SimdJsonOwm::parseLocations(): Location data is empty!" << std::endl;
      return false;
    }
    // add element to result
    locations.emplace_back(loc, w);
  } // for

  return true;
}
#endif // wic_owm_find_location

} // namespace
