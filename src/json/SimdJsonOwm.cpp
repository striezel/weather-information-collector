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

#include "SimdJsonOwm.hpp"
#include <iostream>
#include "../util/NumericPrecision.hpp"

namespace wic
{

bool SimdJsonOwm::parseSingleWeatherItem(const value_type& value, Weather& weather)
{
  bool foundValidParts = false;
  simdjson::dom::element elem;
  auto error = value["main"].get(elem);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type main = value["main"];
    main["temp"].tie(elem, error);
    if (!error && elem.is<double>())
    {
      const double kelvinRaw = elem.get<double>().value();
      weather.setTemperatureKelvin(static_cast<float>(kelvinRaw));
      // Avoid values like 280.9999... K by rounding, if appropriate.
      const float kelvinRounded = NumericPrecision<float>::enforce(weather.temperatureKelvin());
      if (kelvinRounded != weather.temperatureKelvin())
      {
        weather.setTemperatureKelvin(kelvinRounded);
      }
      weather.setTemperatureCelsius(static_cast<float>(kelvinRaw - 273.15));
      // Avoid values like 6.9999... ° C by rounding, if appropriate.
      const float celsiusRounded = NumericPrecision<float>::enforce(weather.temperatureCelsius());
      if (celsiusRounded != weather.temperatureCelsius())
      {
        weather.setTemperatureCelsius(celsiusRounded);
      }
      weather.setTemperatureFahrenheit(static_cast<float>((kelvinRaw - 273.15) * 1.8 + 32.0));
      // Avoid values like 6.9999... ° F by rounding, if appropriate.
      const float fahrenheitRounded = NumericPrecision<float>::enforce(weather.temperatureFahrenheit());
      if (fahrenheitRounded != weather.temperatureFahrenheit())
      {
        weather.setTemperatureFahrenheit(fahrenheitRounded);
      }
    }
    main["pressure"].tie(elem, error);
    if (!error && elem.is<double>())
      weather.setPressure(static_cast<int16_t>(elem.get<double>()));
    main["humidity"].tie(elem, error);
    if (!error && elem.is<uint64_t>())
      weather.setHumidity(static_cast<int8_t>(elem.get<uint64_t>()));
    foundValidParts = true;
  } // if main object
  value["wind"].tie(elem, error);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type wind = elem;
    wind["speed"].tie(elem, error);
    if (!error && elem.is<double>())
      weather.setWindSpeed(static_cast<float>(elem.get<double>().value()));
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
      weather.setCloudiness(static_cast<int8_t>(elem.get<int64_t>().value()));
  } // if clouds object
  value["rain"].tie(elem, error);
  if (!error && elem.type() == simdjson::dom::element_type::OBJECT)
  {
    const value_type rain = elem;
    rain["3h"].tie(elem, error);
    if (!error)
    {
      if (elem.is<double>())
        weather.setRain(static_cast<float>(elem.get<double>().value()));
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
        weather.setSnow(static_cast<float>(elem.get<double>().value()));
    }
    else
    {
      // Empty snow object means zero snow.
      weather.setSnow(0.0f);
    }
  } // if snow object
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
  simdjson::dom::element doc;
  const auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonOwm::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(error) << std::endl;
    return false;
  }

  weather.setJson(json);

  return parseSingleWeatherItem(doc, weather);
}

bool SimdJsonOwm::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  const auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(error) << std::endl;
    return false;
  }

  forecast.setJson(json);

  simdjson::dom::element list;
  const auto e2 = doc["list"].get(list);
  if (e2 || list.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): list is either missing or not an array!" << std::endl;
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
  simdjson::dom::element cnt;
  const auto e3 = doc["cnt"].get(cnt);
  if (e3 || !cnt.is<uint64_t>())
  {
    std::cerr << "Error in SimdJsonOwm::parseForecast(): cnt is either missing or not an integer!" << std::endl;
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
  simdjson::dom::element doc;
  const auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonOwm::parseLocations(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(error) << std::endl;
    return false;
  }

  simdjson::dom::element count;
  const auto e2 = doc["count"].get(count);
  if (e2 || !count.is<int64_t>())
  {
    std::cerr << "Error in SimdJsonOwm::parseLocations(): JSON element for count is missing!" << std::endl;
    return false;
  }
  locations.clear();
  if (count.get<int64_t>().value() == 0)
    return true;

  simdjson::dom::element list;
  const auto e3 = doc["list"].get(list);
  if (e3 || list.type() != simdjson::dom::element_type::ARRAY)
  {
    std::cerr << "Error in SimdJsonOwm::parseLocations(): JSON list element is missing!" << std::endl;
    return false;
  }

  for (const value_type elem : list)
  {
    Location loc;
    simdjson::dom::element val;
    auto err = elem["id"].get(val);
    if (!err && val.is<uint64_t>())
      loc.setOwmId(static_cast<uint32_t>(val.get<uint64_t>()));
    elem["name"].tie(val, err);
    if (!err && val.is<std::string_view>())
      loc.setName(val.get<std::string_view>().value());
    simdjson::dom::element coord;
    const auto errorCoord = elem["coord"].get(coord);
    if (!errorCoord && coord.type() == simdjson::dom::element_type::OBJECT)
    {
      simdjson::dom::element lat;
      const auto errorLat = coord["lat"].get(lat);
      simdjson::dom::element lon;
      const auto errorLon = coord["lon"].get(lon);
      if (!errorLat && !errorLon && lat.is<double>() && lon.is<double>())
      {
        loc.setCoordinates(static_cast<float>(lat.get<double>().value()),
                           static_cast<float>(lon.get<double>().value()));
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
      simdjson::dom::element country;
      const auto errCountry = val["country"].get(country);
      if (!errCountry && country.is<std::string_view>())
        loc.setCountryCode(country.get<std::string_view>().value());
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
