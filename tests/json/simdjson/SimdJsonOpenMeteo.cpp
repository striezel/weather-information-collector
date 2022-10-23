/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
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

#include "../../find_catch.hpp"
#include "../../../src/json/SimdJsonOpenMeteo.hpp"
#include <chrono>
#include <ctime>
#include <stdexcept>

std::chrono::system_clock::time_point toOpenMeteoTime(const int year, const int month, const int day, const int hours, const int minutes)
{
  std::tm tm{};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hours;
  tm.tm_min = minutes;
  tm.tm_sec = 0;
  tm.tm_isdst = -1; // no info on daylight saving time
  const std::time_t tt = std::mktime(&tm);
  if (tt == static_cast<std::time_t>(-1))
    throw std::runtime_error("Conversion with std::mktime() failed!");
  return std::chrono::system_clock::from_time_t(tt);
}

TEST_CASE("Class SimdJsonOpenMeteo")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("", weather) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("{ }", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("successful parsing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": 8.2,
          "winddirection": 105,
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
      // Check data.
      REQUIRE( weather.dataTime() == toOpenMeteoTime(2022, 9, 8, 13, 0) );
      REQUIRE( weather.temperatureCelsius() == 16.8f );
      REQUIRE( weather.temperatureFahrenheit() == 62.24f );
      REQUIRE( weather.temperatureKelvin() == 289.95f );
      REQUIRE( weather.windSpeed() == 8.2f );
      REQUIRE( weather.windDegrees() == 105 );
      REQUIRE( weather.json() == json );
      REQUIRE_FALSE( weather.hasHumidity() );
      REQUIRE_FALSE( weather.hasRain() );
      REQUIRE_FALSE( weather.hasSnow() );
      REQUIRE_FALSE( weather.hasPressure() );
      REQUIRE_FALSE( weather.hasCloudiness() );
    }

    SECTION("failure: current_weather is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: current_weather is not an object")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": "this fails here"
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: temperature is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "windspeed": 8.2,
          "winddirection": 105,
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: temperature is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": "I am a string.",
          "windspeed": 8.2,
          "winddirection": 105,
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: windspeed is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "winddirection": 105,
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: windspeed is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": [ "fails", 1, 2, 3 ],
          "winddirection": 105,
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: winddirection is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": 8.2,
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: winddirection is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": 8.2,
          "winddirection": { "foo": "bar" },
          "weathercode": 61,
          "time": "2022-09-08T13:00"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: time is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": 8.2,
          "winddirection": 105,
          "weathercode": 61
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: time is not a string")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": 8.2,
          "winddirection": 105,
          "weathercode": 61,
          "time": 1234
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }

    SECTION("failure: time is not a valid ISO 8601 date")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 0.3770589828491211,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "current_weather": {
          "temperature": 16.8,
          "windspeed": 8.2,
          "winddirection": 105,
          "weathercode": 61,
          "time": "2022-FA-ILTHE:RE"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseCurrentWeather(json, weather) );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("", forecast) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("{ }", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("    ", forecast) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("\n", forecast) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("\r", forecast) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast("\t\t\t\t", forecast) );
    }

    SECTION("successful parsing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
      // Check data.
      REQUIRE( forecast.data().size() == 5 );
      REQUIRE( forecast.json() == json );
      // Check first element.
      REQUIRE( forecast.data()[0].dataTime() == toOpenMeteoTime(2022, 9, 8, 0, 0) );
      REQUIRE( forecast.data()[0].temperatureCelsius() == 17.7f );
      REQUIRE( forecast.data()[0].temperatureFahrenheit() == 63.86f );
      REQUIRE( forecast.data()[0].temperatureKelvin() == 290.85f );
      REQUIRE( forecast.data()[0].humidity() == 61 );
      REQUIRE( forecast.data()[0].rain() == 0.0f );
      REQUIRE( forecast.data()[0].snow() == 0.0f );
      REQUIRE( forecast.data()[0].pressure() == 1012 );
      REQUIRE( forecast.data()[0].windSpeed() == 2.62f );
      REQUIRE( forecast.data()[0].windDegrees() == 108 );
      REQUIRE( forecast.data()[0].cloudiness() == 100 );
      // Check second element.
      REQUIRE( forecast.data()[1].dataTime() == toOpenMeteoTime(2022, 9, 8, 1, 0) );
      REQUIRE( forecast.data()[1].temperatureCelsius() == 17.1f );
      REQUIRE( forecast.data()[1].temperatureFahrenheit() == 62.78f );
      REQUIRE( forecast.data()[1].temperatureKelvin() == 290.25f );
      REQUIRE( forecast.data()[1].humidity() == 61 );
      REQUIRE( forecast.data()[1].rain() == 0.1f );
      REQUIRE( forecast.data()[1].snow() == 0.0f );
      REQUIRE( forecast.data()[1].pressure() == 1011 );
      REQUIRE( forecast.data()[1].windSpeed() == 2.9f );
      REQUIRE( forecast.data()[1].windDegrees() == 90 );
      REQUIRE( forecast.data()[1].cloudiness() == 99 );
      // Check third element.
      REQUIRE( forecast.data()[2].dataTime() == toOpenMeteoTime(2022, 9, 8, 2, 0) );
      REQUIRE( forecast.data()[2].temperatureCelsius() == 16.6f );
      REQUIRE( forecast.data()[2].temperatureFahrenheit() == 61.88f );
      REQUIRE( forecast.data()[2].temperatureKelvin() == 289.75f );
      REQUIRE( forecast.data()[2].humidity() == 62 );
      REQUIRE( forecast.data()[2].rain() == 1.1f );
      REQUIRE( forecast.data()[2].snow() == 0.0f );
      REQUIRE( forecast.data()[2].pressure() == 1011 );
      REQUIRE( forecast.data()[2].windSpeed() == 2.4f );
      REQUIRE( forecast.data()[2].windDegrees() == 88 );
      REQUIRE( forecast.data()[2].cloudiness() == 93 );
      // Check fourth element.
      REQUIRE( forecast.data()[3].dataTime() == toOpenMeteoTime(2022, 9, 8, 3, 0) );
      REQUIRE( forecast.data()[3].temperatureCelsius() == 16.2f );
      REQUIRE( forecast.data()[3].temperatureFahrenheit() == 61.16f );
      REQUIRE( forecast.data()[3].temperatureKelvin() == 289.35f );
      REQUIRE( forecast.data()[3].humidity() == 61 );
      REQUIRE( forecast.data()[3].rain() == 0.5f );
      REQUIRE( forecast.data()[3].snow() == 0.0f );
      REQUIRE( forecast.data()[3].pressure() == 1010 );
      REQUIRE( forecast.data()[3].windSpeed() == 2.55f );
      REQUIRE( forecast.data()[3].windDegrees() == 79 );
      REQUIRE( forecast.data()[3].cloudiness() == 95 );
      // Check fifth element.
      REQUIRE( forecast.data()[4].dataTime() == toOpenMeteoTime(2022, 9, 8, 4, 0) );
      REQUIRE( forecast.data()[4].temperatureCelsius() == 15.8f );
      REQUIRE( forecast.data()[4].temperatureFahrenheit() == 60.44f );
      REQUIRE( forecast.data()[4].temperatureKelvin() == 288.95f );
      REQUIRE( forecast.data()[4].humidity() == 60 );
      REQUIRE( forecast.data()[4].rain() == 1.3f );
      REQUIRE( forecast.data()[4].snow() == 0.0f );
      REQUIRE( forecast.data()[4].pressure() == 1010 );
      REQUIRE( forecast.data()[4].windSpeed() == 2.72f );
      REQUIRE( forecast.data()[4].windDegrees() == 84 );
      REQUIRE( forecast.data()[4].cloudiness() == 1 );
    }

    SECTION("wrong time unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "unixtime",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong temperature unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°F",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong humidity unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "fail",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong rain unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "inch",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong snow unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "inch",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong pressure unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "fail",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong cloud cover unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "fail",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong wind speed unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "km/h",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("wrong wind direction unit causes failure")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "fail"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: hourly element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: hourly element is not an object")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": "not an object here"
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: time element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: time element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": "2022-09-08T00:00",
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: time element is an empty array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [ ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: time array's element is not a string")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", 12345, "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: time array's element is not a valid date")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-FA-ILTHE:RE", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: temperature_2m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: temperature_2m element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": 17.7,
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: temperature_2m array's element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, "fail", 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in temperature_2m does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: relativehumidity_2m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: relativehumidity_2m is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": "failure",
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: relativehumidity_2m array's element is not an integer")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, "hmm ... failure", 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in relativehumidity_2m does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60,
            64
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: rain element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: rain element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": 1.23,
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: rain array's element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ "failure", 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in rain does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: snowfall element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: snowfall element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": "fails here",
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: snowfall array's element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, "fail", 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in snowfall does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: pressure_msl element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: pressure_msl element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": { "a": "fail", "b": "fail", "c": "fail", "d": "fail", "e": "fail" },
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: pressure_msl array's element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ "fail", 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in pressure_msl does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: cloudcover element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: cloudcover element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": "fail: not an array",
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: cloudcover array element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, "foo!", 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in cloudcover does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: windspeed_10m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: windspeed_10m element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": "123fail",
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: windspeed_10m array element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, { "fail": "here" }, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, 90, 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in windspeed_10m does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.72
          ],
          "winddirection_10m": [
            108,
            90,
            88,
            79,
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: winddirection_10m element is missing")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: winddirection_10m element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": "I am not an array."
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: winddirection_10m array element is not a number")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00", "2022-09-08T01:00", "2022-09-08T02:00", "2022-09-08T03:00", "2022-09-08T04:00"
          ],
          "temperature_2m": [ 17.7, 17.1, 16.6, 16.2, 15.8 ],
          "relativehumidity_2m": [ 61, 61, 62, 61, 60 ],
          "precipitation": [ 0, 0.3, 0.1, 1.1, 2.6 ],
          "rain": [ 0, 0.1, 1.1, 0.5, 1.3 ],
          "snowfall": [ 0, 0, 0, 0, 0 ],
          "pressure_msl": [ 1012.1, 1011.5, 1011.3, 1010.2, 1010.7 ],
          "surface_pressure": [ 1007.6, 1007, 1006.8, 1005.7, 1006.2 ],
          "cloudcover": [ 100, 99, 93, 95, 1 ],
          "windspeed_10m": [ 2.62, 2.9, 2.4, 2.55, 2.72 ],
          "winddirection_10m": [ 108, "fail", 88, 79, 84 ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }

    SECTION("failure: number of elements in winddirection_10m does not match time elements")
    {
      const std::string json = R"json(
      {
        "latitude": 52.52,
        "longitude": 13.419998,
        "generationtime_ms": 1.1960268020629883,
        "utc_offset_seconds": 0,
        "timezone": "GMT",
        "timezone_abbreviation": "GMT",
        "elevation": 38,
        "hourly_units": {
          "time": "iso8601",
          "temperature_2m": "°C",
          "relativehumidity_2m": "%",
          "precipitation": "mm",
          "rain": "mm",
          "showers": "mm",
          "snowfall": "cm",
          "pressure_msl": "hPa",
          "surface_pressure": "hPa",
          "cloudcover": "%",
          "windspeed_10m": "m/s",
          "winddirection_10m": "°"
        },
        "hourly": {
          "time": [
            "2022-09-08T00:00",
            "2022-09-08T01:00",
            "2022-09-08T02:00",
            "2022-09-08T03:00",
            "2022-09-08T04:00"
          ],
          "temperature_2m": [
            17.7,
            17.1,
            16.6,
            16.2,
            15.8
          ],
          "relativehumidity_2m": [
            61,
            61,
            62,
            61,
            60
          ],
          "precipitation": [
            0,
            0.3,
            0.1,
            1.1,
            2.6
          ],
          "rain": [
            0,
            0.1,
            1.1,
            0.5,
            1.3
          ],
          "snowfall": [
            0,
            0,
            0,
            0,
            0
          ],
          "pressure_msl": [
            1012.1,
            1011.5,
            1011.3,
            1010.2,
            1010.7
          ],
          "surface_pressure": [
            1007.6,
            1007,
            1006.8,
            1005.7,
            1006.2
          ],
          "cloudcover": [
            100,
            99,
            93,
            95,
            1
          ],
          "windspeed_10m": [
            2.62,
            2.9,
            2.4,
            2.55,
            2.72
          ],
          "winddirection_10m": [
            84
          ]
        }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseForecast(json, forecast) );
    }
  }

  SECTION("parseLocations")
  {
    std::vector<Location> locations;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("", locations) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("{ }", locations) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("    ", locations) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("\n", locations) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("\r", locations) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("\r\n\r\n", locations) );
      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations("\t\t\t\t", locations) );
    }

    SECTION("successful parsing with zero elements")
    {
      const std::string json = R"json(
      {
        "generationtime_ms": 0.6788969
      }
      )json";

      REQUIRE( SimdJsonOpenMeteo::parseLocations(json, locations) );
      REQUIRE( locations.size() == 0 );
    }

    SECTION("successful parsing with some matching elements")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "latitude": 51.5768,
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          },
          {
            "id": 2179661,
            "name": "Wanstead",
            "latitude": -40.13333,
            "longitude": 176.53333,
            "elevation": 113,
            "feature_code": "PPL",
            "country_code": "NZ",
            "admin1_id": 2190146,
            "admin2_id": 7910034,
            "timezone": "Pacific/Auckland",
            "country_id": 2186224,
            "country": "New Zealand",
            "admin1": "Hawke's Bay",
            "admin2": "Central Hawke's Bay District"
          },
          {
            "id": 3373438,
            "name": "Wanstead",
            "latitude": 13.13979,
            "longitude": -59.62782,
            "elevation": 103,
            "feature_code": "PPL",
            "country_code": "BB",
            "admin1_id": 3373557,
            "timezone": "America/Barbados",
            "country_id": 3374084,
            "country": "Barbados",
            "admin1": "Saint Michael"
          },
          {
            "id": 12048316,
            "name": "Wanstead",
            "latitude": 51.56721,
            "longitude": 0.03282,
            "elevation": 19,
            "feature_code": "PPLX",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE( SimdJsonOpenMeteo::parseLocations(json, locations) );
      REQUIRE( locations.size() == 4 );

      REQUIRE_FALSE( locations[0].hasOwmId() );
      REQUIRE( locations[0].latitude() == 51.5768f );
      REQUIRE( locations[0].longitude() == 0.02463f );
      REQUIRE( locations[0].name() == "Wanstead" );
      REQUIRE( locations[0].countryCode() == "GB" );
      REQUIRE( locations[0].postcode().empty() );

      REQUIRE_FALSE( locations[1].hasOwmId() );
      REQUIRE( locations[1].latitude() == -40.13333f );
      REQUIRE( locations[1].longitude() == 176.53333f );
      REQUIRE( locations[1].name() == "Wanstead" );
      REQUIRE( locations[1].countryCode() == "NZ" );
      REQUIRE( locations[1].postcode().empty() );

      REQUIRE_FALSE( locations[2].hasOwmId() );
      REQUIRE( locations[2].latitude() == 13.13979f );
      REQUIRE( locations[2].longitude() == -59.62782f );
      REQUIRE( locations[2].name() == "Wanstead" );
      REQUIRE( locations[2].countryCode() == "BB" );
      REQUIRE( locations[2].postcode().empty() );

      REQUIRE_FALSE( locations[3].hasOwmId() );
      REQUIRE( locations[3].latitude() == 51.56721f );
      REQUIRE( locations[3].longitude() == 0.03282f );
      REQUIRE( locations[3].name() == "Wanstead" );
      REQUIRE( locations[3].countryCode() == "GB" );
      REQUIRE( locations[3].postcode().empty() );
    }

    SECTION("failure: results is not an array")
    {
      const std::string json = R"json(
      {
        "results": "Not an array",
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: name is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,

            "latitude": 51.5768,
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          },
          {
            "id": 2179661,
            "name": "Wanstead",
            "latitude": -40.13333,
            "longitude": 176.53333,
            "elevation": 113,
            "feature_code": "PPL",
            "country_code": "NZ",
            "admin1_id": 2190146,
            "admin2_id": 7910034,
            "timezone": "Pacific/Auckland",
            "country_id": 2186224,
            "country": "New Zealand",
            "admin1": "Hawke's Bay",
            "admin2": "Central Hawke's Bay District"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: name is not a string")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": { "fail": true },
            "latitude": 51.5768,
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          },
          {
            "id": 2179661,
            "name": "Wanstead",
            "latitude": -40.13333,
            "longitude": 176.53333,
            "elevation": 113,
            "feature_code": "PPL",
            "country_code": "NZ",
            "admin1_id": 2190146,
            "admin2_id": 7910034,
            "timezone": "Pacific/Auckland",
            "country_id": 2186224,
            "country": "New Zealand",
            "admin1": "Hawke's Bay",
            "admin2": "Central Hawke's Bay District"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: latitude is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: latitude is not a number")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "latitude": "fails here",
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: longitude is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "latitude": 51.5768,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: longitude is not a number")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "latitude": 51.5768,
            "longitude": "fails here",
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: country_code is missing")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "latitude": 51.5768,
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          },
          {
            "id": 2179661,
            "name": "Wanstead",
            "latitude": -40.13333,
            "longitude": 176.53333,
            "elevation": 113,
            "feature_code": "PPL",
            "admin1_id": 2190146,
            "admin2_id": 7910034,
            "timezone": "Pacific/Auckland",
            "country_id": 2186224,
            "country": "New Zealand",
            "admin1": "Hawke's Bay",
            "admin2": "Central Hawke's Bay District"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }

    SECTION("failure: country_code is not a string")
    {
      const std::string json = R"json(
      {
        "results": [
          {
            "id": 2634803,
            "name": "Wanstead",
            "latitude": 51.5768,
            "longitude": 0.02463,
            "elevation": 33,
            "feature_code": "PPL",
            "country_code": "GB",
            "admin1_id": 6269131,
            "admin2_id": 2648110,
            "admin3_id": 3333185,
            "timezone": "Europe/London",
            "population": 11543,
            "country_id": 2635167,
            "country": "United Kingdom",
            "admin1": "England",
            "admin2": "Greater London",
            "admin3": "Redbridge"
          },
          {
            "id": 2179661,
            "name": "Wanstead",
            "latitude": -40.13333,
            "longitude": 176.53333,
            "elevation": 113,
            "feature_code": "PPL",
            "country_code": 123456,
            "admin1_id": 2190146,
            "admin2_id": 7910034,
            "timezone": "Pacific/Auckland",
            "country_id": 2186224,
            "country": "New Zealand",
            "admin1": "Hawke's Bay",
            "admin2": "Central Hawke's Bay District"
          }
        ],
        "generationtime_ms": 0.41508675
      }
      )json";

      REQUIRE_FALSE( SimdJsonOpenMeteo::parseLocations(json, locations) );
    }
  }
}
