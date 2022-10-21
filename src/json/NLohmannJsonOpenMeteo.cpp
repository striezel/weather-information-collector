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

#include "NLohmannJsonOpenMeteo.hpp"
#include <iostream>
#include "OpenMeteoFunctions.hpp"
#include "../util/NumericPrecision.hpp"

namespace wic
{

bool NLohmannJsonOpenMeteo::parseCurrentWeather(const std::string& json, Weather& weather)
{
  nlohmann::json root; // will contain the root value after parsing.
  try
  {
    root = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  weather.setJson(json);

  if (root.empty())
    return false;

  // Current weather data is located in the current_weather object below the root.
  auto find = root.find("current_weather");
  if (find == root.end() || !find->is_object())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'current_weather' is either missing or not an object!"
              << std::endl;
    return false;
  }
  nlohmann::json& current_weather = *find;
  find = current_weather.find("temperature");
  if (find == current_weather.end() || !find->is_number())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'temperature' is either missing or not a floating "
              << "point value!" << std::endl;
    return false;
  }
  weather = Weather();
  const double celsius = find->get<double>();
  weather.setTemperatureCelsius(celsius);
  weather.setTemperatureFahrenheit(celsius * 1.8 + 32.0);
  weather.setTemperatureKelvin(celsius + 273.15);

  find = current_weather.find("windspeed");
  if (find == current_weather.end() || !find->is_number())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'windspeed' is either missing or not a floating "
              << "point value!" << std::endl;
    return false;
  }
  weather.setWindSpeed(find->get<float>());

  find = current_weather.find("winddirection");
  if (find == current_weather.end() || !find->is_number_unsigned())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'winddirection' is either missing or not an integer!"
              << std::endl;
    return false;
  }
  weather.setWindDegrees(find->get<int>());

  find = current_weather.find("time");
  if (find == current_weather.end() || !find->is_string())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseCurrentWeather(): JSON "
              << "element 'time' is either missing or not a string!"
              << std::endl;
    return false;
  }
  const auto dt = isoStringToTime(find->get<std::string>());
  if (!dt.has_value())
  {
    return false;
  }
  weather.setDataTime(dt.value());

  // There is no more data in current weather in Open-Meteo.
  weather.setJson(json);
  return true;
}

std::optional<std::string> NLohmannJsonOpenMeteo::hourlyUnitCheck(const nlohmann::json& doc)
{
  auto find = doc.find("hourly_units");
  if (find == doc.end() || !find->is_object())
  {
    return std::optional("Data about hourly units is missing or not a JSON object!");
  }
  nlohmann::json hourly_units = *find;
  // "time" is usually "iso8601", values are something like "2022-10-12T04:00" then.
  // However, "unixtime" uses Unix timestamps instead.
  find = hourly_units.find("time");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "iso8601")
    return std::optional("Time format is missing or not in ISO 8601 format.");
  // "temperature_2m" should be "°C" for Celsius scale.
  find = hourly_units.find("temperature_2m");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "°C")
    return std::optional("Temperature unit is missing or not degrees Celsius.");
  // "relativehumidity_2m" is always "%".
  find = hourly_units.find("relativehumidity_2m");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "%")
    return std::optional("Relative humidity unit is missing or not a percentage.");
  // "rain" should be "mm" for millimeters.
  find = hourly_units.find("rain");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "mm")
    return std::optional("Rain unit is missing or not millimeters.");
  // "showers" should be "mm" for millimeters.
  find = hourly_units.find("showers");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "mm")
    return std::optional("Shower unit is missing or not millimeters.");
  // "snowfall" should be "cm" for centimeters.
  find = hourly_units.find("snowfall");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "cm")
    return std::optional("Snowfall unit is missing or not centimeters.");
  // "pressure_msl" should be "hPa" for hectopascal.
  find = hourly_units.find("pressure_msl");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "hPa")
    return std::optional("Pressure unit is missing or not hPa.");
  // "surface_pressure" should be "hPa" for hectopascal.
  find = hourly_units.find("surface_pressure");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "hPa")
    return std::optional("Pressure unit is missing or not hPa.");
  // "cloudcover" is always "%".
  find = hourly_units.find("cloudcover");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "%")
    return std::optional("Cloud cover unit is missing or not a percentage.");
  // "windspeed_10m" should be "m/s" for meters per second.
  find = hourly_units.find("windspeed_10m");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "m/s")
    return std::optional("Wind speed unit is missing or not m/s.");
  // "winddirection_10m" should be "°" for degrees.
  find = hourly_units.find("winddirection_10m");
  if (find == hourly_units.end() || !find->is_string() || find->get<std::string>() != "°")
    return std::optional("Wind direction unit is missing or not degrees.");

  // All units match the expected values.
  return std::nullopt;
}

bool NLohmannJsonOpenMeteo::parseTemperature(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("temperature_2m");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.temperature_2m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto temperature = *find;
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
    if (!elem.is_number())
    {
      std::cerr << "Error: Temperature element is not a floating point value!" << std::endl;
      return false;
    }
    const double celsius = elem.get<double>();
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

bool NLohmannJsonOpenMeteo::parseHumidity(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("relativehumidity_2m");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.relativehumidity_2m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto humidity = *find;
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
    if (!elem.is_number_integer())
    {
      std::cerr << "Error: Humidity element is not an integer value!" << std::endl;
      return false;
    }
    data[idx].setHumidity(elem.get<int>());
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parseRain(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("rain");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.rain' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto rain = *find;
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
    if (!elem.is_number())
    {
      std::cerr << "Error: Rain element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setRain(elem.get<double>());
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parseSnowfall(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("snowfall");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.snowfall' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto snowfall = *find;
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
    if (!elem.is_number())
    {
      std::cerr << "Error: Snowfall element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setSnow(elem.get<double>() / 0.7);
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parsePressure(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("pressure_msl");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.pressure_msl' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto pressure_msl = *find;
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
    if (!elem.is_number())
    {
      std::cerr << "Error: Pressure element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setPressure(elem.get<double>());
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parseCloudCover(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("cloudcover");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.cloudcover' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto clouds = *find;
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
    if (!elem.is_number_integer())
    {
      std::cerr << "Error: Cloud cover element is not an integer value!" << std::endl;
      return false;
    }
    data[idx].setCloudiness(elem.get<int>());
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parseWindSpeed(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("windspeed_10m");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.windspeed_10m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto windspeed_10m = *find;
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
    if (!elem.is_number())
    {
      std::cerr << "Error: Pressure element is not a floating point value!" << std::endl;
      return false;
    }
    data[idx].setWindSpeed(elem.get<double>());
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parseWindDirection(const nlohmann::json& hourly, std::vector<Weather>& data)
{
  const auto find = hourly.find("winddirection_10m");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.winddirection_10m' is either missing or not an array!"
              << std::endl;
    return false;
  }
  const auto winddirection_10m = *find;
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
    if (!elem.is_number_integer())
    {
      std::cerr << "Error: Wind direction element is not an integer value!" << std::endl;
      return false;
    }
    data[idx].setWindDegrees(elem.get<int>());
    ++idx;
  }

  return true;
}

bool NLohmannJsonOpenMeteo::parseForecast(const std::string& json, Forecast& forecast)
{
  nlohmann::json doc; // will contain the root value after parsing.
  try
  {
    doc = nlohmann::json::parse(json);
  }
  catch(const nlohmann::json::parse_error& ex)
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): Unable to parse JSON data!" << std::endl
              << "Parser error: " << ex.what() << std::endl;
    return false;
  }

  forecast.setJson(json);
  if (doc.empty())
    return false;

  const auto optional_error = hourlyUnitCheck(doc);
  if (optional_error.has_value())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): Unexpected unit values found!"
              << std::endl << optional_error.value() << std::endl;
    return false;
  }

  auto find = doc.find("hourly");
  if (find == doc.end() || !find->is_object())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly' is either missing or not an object!" << std::endl;
    return false;
  }
  nlohmann::json& hourly = *find;

  find = hourly.find("time");
  if (find == hourly.end() || !find->is_array())
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
              << " 'hourly.time' is either missing or not an array!"
              << std::endl;
    return false;
  }

  const auto time = *find;
  const auto element_count = time.size();
  if (element_count == 0)
  {
    std::cerr << "Error in NLohmannJsonOpenMeteo::parseForecast(): JSON element"
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
    const auto dt = isoStringToTime(std::string(elem.get<std::string>()));
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

} // namespace
