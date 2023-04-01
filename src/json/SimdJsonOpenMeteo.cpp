/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2022, 2023  Dirk Stolle

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

#include "SimdJsonOpenMeteo.hpp"
#include <iostream>
#include "OpenMeteoFunctions.hpp"
#include "../util/NumericPrecision.hpp"

namespace wic
{

bool SimdJsonOpenMeteo::parseCurrentWeather(const std::string& json, Weather& weather)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseCurrentWeather(): Unable to parse JSON data!"
              << std::endl << "Parser error: " << simdjson::error_message(error)
              << std::endl;
    return false;
  }

  simdjson::dom::element current_weather;
  error = doc["current_weather"].get(current_weather);
  if (error || !current_weather.is_object())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'current_weather' is either missing or not an object!"
              << std::endl;
    return false;
  }

  simdjson::dom::element elem;
  error = current_weather["temperature"].get(elem);
  if (error || !elem.is_double())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'temperature' is either missing or not a floating "
              << "point value!" << std::endl;
    return false;
  }
  const double celsius = elem.get<double>().value();
  weather.setTemperatureCelsius(celsius);
  weather.setTemperatureFahrenheit(celsius * 1.8 + 32.0);
  weather.setTemperatureKelvin(celsius + 273.15);

  error = current_weather["windspeed"].get(elem);
  if (error || !elem.is_double())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'windspeed' is either missing or not a floating "
              << "point value!" << std::endl;
    return false;
  }
  weather.setWindSpeed(elem.get<double>().value());

  error = current_weather["winddirection"].get(elem);
  if (error || !elem.is_number())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'winddirection' is either missing or not a number!"
              << std::endl;
    return false;
  }
  weather.setWindDegrees(elem.get<double>().value());

  error = current_weather["time"].get(elem);
  if (error || !elem.is_string())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'time' is either missing or not a string!"
              << std::endl;
    return false;
  }
  const auto dt = isoStringToTime(std::string(elem.get_string().value()));
  if (!dt.has_value())
  {
    return false;
  }
  weather.setDataTime(dt.value());

  // There is no more data in current weather in Open-Meteo.
  weather.setJson(json);
  return true;
}

std::optional<std::string> SimdJsonOpenMeteo::hourlyUnitCheck(const simdjson::dom::element& doc)
{
  simdjson::dom::element hourly_units;
  auto error = doc["hourly_units"].get(hourly_units);
  if (error || !hourly_units.is_object())
  {
    return std::optional("Data about hourly units is missing or not a JSON object!");
  }
  simdjson::dom::element elem;
  // "time" is usually "iso8601", values are something like "2022-10-12T04:00" then.
  // However, "unixtime" uses Unix timestamps instead.
  hourly_units["time"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "iso8601")
    return std::optional("Time format is missing or not in ISO 8601 format.");
  // "temperature_2m" should be "°C" for Celsius scale.
  hourly_units["temperature_2m"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "°C")
    return std::optional("Temperature unit is missing or not degrees Celsius.");
  // "relativehumidity_2m" is always "%".
  hourly_units["relativehumidity_2m"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "%")
    return std::optional("Relative humidity unit is missing or not a percentage.");
  // "rain" should be "mm" for millimeters.
  hourly_units["rain"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "mm")
    return std::optional("Rain unit is missing or not millimeters.");
  // "showers" should be "mm" for millimeters.
  hourly_units["showers"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "mm")
    return std::optional("Shower unit is missing or not millimeters.");
  // "snowfall" should be "cm" for centimeters.
  hourly_units["snowfall"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "cm")
    return std::optional("Snowfall unit is missing or not centimeters.");
  // "pressure_msl" should be "hPa" for hectopascal.
  hourly_units["pressure_msl"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "hPa")
    return std::optional("Pressure unit is missing or not hPa.");
  // "surface_pressure" should be "hPa" for hectopascal.
  hourly_units["surface_pressure"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "hPa")
    return std::optional("Pressure unit is missing or not hPa.");
  // "cloudcover" is always "%".
  hourly_units["cloudcover"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "%")
    return std::optional("Cloud cover unit is missing or not a percentage.");
  // "windspeed_10m" should be "m/s" for meters per second.
  hourly_units["windspeed_10m"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "m/s")
    return std::optional("Wind speed unit is missing or not m/s.");
  // "winddirection_10m" should be "°" for degrees.
  hourly_units["winddirection_10m"].tie(elem, error);
  if (error || !elem.is_string() || elem.get_string().value() != "°")
    return std::optional("Wind direction unit is missing or not degrees.");

  // All units match the expected values.
  return std::nullopt;
}

bool SimdJsonOpenMeteo::parseTemperature(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["temperature_2m"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.temperature_2m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array temperature = element.get_array().value();
  if (temperature.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.temperature_2m does not "
              << "match the expected value of " << data.size() << ". It is "
              << temperature.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: temperature)
  {
    if (!elem.is_double())
    {
      std::cerr << "Error: Temperature element is not a floating point value!" << std::endl;
      return false;
    }
    const double celsius = elem.get_double().value();
    data[idx].setTemperatureCelsius(celsius);
    data[idx].setTemperatureFahrenheit(celsius * 1.8 + 32.0);
    // Avoid values like 6.9999... ° F by rounding, if appropriate.
    const float fahrenheitRounded = NumericPrecision<float>::enforce(data[idx].temperatureFahrenheit());
    if (fahrenheitRounded != data[idx].temperatureFahrenheit())
    {
      data[idx].setTemperatureFahrenheit(fahrenheitRounded);
    }
    data[idx].setTemperatureKelvin(celsius + 273.15);
    // Avoid values like 296.9999... K by rounding, if appropriate.
    const float kelvinRounded = NumericPrecision<float>::enforce(data[idx].temperatureKelvin());
    if (kelvinRounded != data[idx].temperatureKelvin())
    {
      data[idx].setTemperatureKelvin(kelvinRounded);
    }
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseHumidity(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["relativehumidity_2m"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.relativehumidity_2m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array humidity = element.get_array().value();
  if (humidity.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.relativehumidity_2m does not "
              << "match the expected value of " << data.size() << ". It is "
              << humidity.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: humidity)
  {
    if (!elem.is<int64_t>())
    {
      std::cerr << "Error: Humidity element is not an integer value!" << std::endl;
      return false;
    }
    data[idx].setHumidity(elem.get<int64_t>().value());
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseRain(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["rain"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.rain' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array rain = element.get_array().value();
  if (rain.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.rain does not "
              << "match the expected value of " << data.size() << ". It is "
              << rain.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: rain)
  {
    if (!elem.is<double>())
    {
      std::cerr << "Error: Rain element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setRain(elem.get<double>().value());
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseSnowfall(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["snowfall"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.snowfall' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array snowfall = element.get_array().value();
  if (snowfall.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.snowfall does not "
              << "match the expected value of " << data.size() << ". It is "
              << snowfall.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: snowfall)
  {
    if (!elem.is<double>())
    {
      std::cerr << "Error: Snowfall element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setSnow(elem.get<double>().value() / 0.7);
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parsePressure(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["pressure_msl"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.pressure_msl' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array pressure_msl = element.get_array().value();
  if (pressure_msl.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.pressure_msl does not "
              << "match the expected value of " << data.size() << ". It is "
              << pressure_msl.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: pressure_msl)
  {
    if (!elem.is<double>())
    {
      std::cerr << "Error: Pressure element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setPressure(elem.get<double>().value());
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseCloudCover(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["cloudcover"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.cloudcover' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array clouds = element.get_array().value();
  if (clouds.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.cloudcover does not "
              << "match the expected value of " << data.size() << ". It is "
              << clouds.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: clouds)
  {
    if (!elem.is<int64_t>())
    {
      std::cerr << "Error: Cloud cover element is not an integer value!" << std::endl;
      return false;
    }
    data[idx].setCloudiness(elem.get<int64_t>().value());
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseWindSpeed(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["windspeed_10m"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.windspeed_10m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array windspeed_10m = element.get_array().value();
  if (windspeed_10m.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.windspeed_10m does not "
              << "match the expected value of " << data.size() << ". It is "
              << windspeed_10m.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: windspeed_10m)
  {
    if (!elem.is<double>())
    {
      std::cerr << "Error: Wind speed element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setWindSpeed(elem.get<double>().value());
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseWindDirection(const simdjson::dom::element& hourly, std::vector<Weather>& data)
{
  simdjson::dom::element element;
  const auto error = hourly["winddirection_10m"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element "
              << "'hourly.winddirection_10m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array winddirection_10m = element.get_array().value();
  if (winddirection_10m.size() != data.size())
  {
    std::cerr << "Error: Number of elements in hourly.winddirection_10m does not "
              << "match the expected value of " << data.size() << ". It is "
              << winddirection_10m.size() << " instead." << std::endl;
    return false;
  }
  std::size_t idx = 0;
  for (const auto& elem: winddirection_10m)
  {
    if (!elem.is<int64_t>())
    {
      std::cerr << "Error: Wind direction element is not an integer value!" << std::endl;
      return false;
    }
    data[idx].setWindDegrees(elem.get<int64_t>().value());
    ++idx;
  }

  return true;
}

bool SimdJsonOpenMeteo::parseForecast(const std::string& json, Forecast& forecast)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): Unable to parse JSON data!"
              << std::endl << "Parser error: " << simdjson::error_message(error)
              << std::endl;
    return false;
  }

  forecast.setJson(json);

  const auto optional_error = hourlyUnitCheck(doc);
  if (optional_error.has_value())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): Unexpected unit values found!"
              << std::endl << optional_error.value() << std::endl;
    return false;
  }

  simdjson::dom::element hourly;
  error = doc["hourly"].get(hourly);
  if (error || !hourly.is_object())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly' is either missing or not an object!" << std::endl;
    return false;
  }

  simdjson::dom::element element;
  error = hourly["time"].get(element);
  if (error || !element.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.time' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const simdjson::dom::array time = element.get_array().value();
  const auto element_count = time.size();
  if (element_count == 0)
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.time' is an empty array!" << std::endl;
    return false;
  }
  std::vector<Weather> data;
  for (const auto& elem: time)
  {
    if (!elem.is_string())
    {
      std::cerr << "Error: Time element is not a string!" << std::endl;
      return false;
    }
    const auto dt = isoStringToTime(std::string(elem.get_string().value()));
    if (!dt.has_value())
    {
      return false;
    }
    Weather weather;
    weather.setDataTime(dt.value());
    data.emplace_back(weather);
  }

  if (!parseTemperature(hourly, data))
    return false;
  if (!parseHumidity(hourly, data))
    return false;
  if (!parseRain(hourly, data))
    return false;
  if (!parseSnowfall(hourly, data))
    return false;
  if (!parsePressure(hourly, data))
    return false;
  if (!parseCloudCover(hourly, data))
    return false;
  if (!parseWindSpeed(hourly, data))
    return false;
  if (!parseWindDirection(hourly, data))
    return false;

  forecast.setData(data);
  return true;
}

#ifdef wic_openmeteo_find_location
bool SimdJsonOpenMeteo::parseLocations(const std::string& json, std::vector<Location>& locations)
{
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(json).get(doc);
  if (error)
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseLocations(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << simdjson::error_message(error) << std::endl;
    return false;
  }

  locations.clear();

  simdjson::dom::element results;
  error = doc["results"].get(results);
  if (error)
  {
    error = doc["generationtime_ms"].get(results);
    if (error || !results.is_number())
    {
      std::cerr << "Error in SimdJsonOpenMeteo::parseLocations(): JSON data "
                << "is not a geocoding API result." << std::endl;
      return false;
    }

    // No results array means no match was found.
    // This is a valid outcome.
    return true;
  }
  if (!results.is_array())
  {
    std::cerr << "Error in SimdJsonOpenMeteo::parseLocations(): JSON element"
              << " 'results' is not an array!" << std::endl;
    return false;
  }
  for (const auto& elem: results)
  {
    if (!elem.is_object())
    {
      std::cerr << "Error in SimdJsonOpenMeteo::parseLocations(): Array element"
              << " of 'results' is not an object!" << std::endl;
      return false;
    }
    Location loc;
    simdjson::dom::element value;
    error = elem["name"].get(value);
    if (error || !value.is_string())
    {
      std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'name' is either missing or not a string!" << std::endl;
      return false;
    }
    loc.setName(value.get_string().value());

    error = elem["country_code"].get(value);
    if (error || !value.is_string())
    {
      std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'country_code' is either missing or not a string!" << std::endl;
      return false;
    }
    loc.setCountryCode(value.get_string().value());

    error = elem["latitude"].get(value);
    if (error || !value.is_number())
    {
      std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'latitude' is either missing or not a number!" << std::endl;
      return false;
    }
    loc.setCoordinates(value.get_double().value(), 0.0);

    error = elem["longitude"].get(value);
    if (error || !value.is_number())
    {
      std::cerr << "Error in SimdJsonOpenMeteo::parseForecast(): JSON element"
              << " 'longitude' is either missing or not a number!" << std::endl;
      return false;
    }
    loc.setCoordinates(loc.latitude(), value.get_double().value());

    locations.emplace_back(loc);
  }

  return true;
}
#endif // wic_openmeteo_find_location

} // namespace
