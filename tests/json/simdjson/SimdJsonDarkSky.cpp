/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2021, 2022  Dirk Stolle

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
#include "../../../src/json/SimdJsonDarkSky.hpp"

TEST_CASE("Class SimdJsonDarkSky")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather("", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("no 'currently' element")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset":-7
      }
      )json";
      REQUIRE_FALSE( SimdJsonDarkSky::parseCurrentWeather(json, weather) );
    }

    SECTION("current weather data only")
    {
      const std::string json = R"json(
      {
        "latitude": 52.516,
        "longitude": 13.378,
        "timezone": "Europe/Berlin",
        "currently": {
          "time": 1533403488,
          "summary": "Partly Cloudy",
          "icon": "partly-cloudy-day",
          "precipIntensity": 0.061,
          "precipProbability": 0.02,
          "precipType": "rain",
          "temperature": 30.49,
          "apparentTemperature": 30.49,
          "dewPoint": 15.21,
          "humidity": 0.4,
          "pressure": 1015.35,
          "windSpeed": 2.8,
          "windGust": 4.63,
          "windBearing": 329,
          "cloudCover": 0.33,
          "uvIndex": 0,
          "visibility": 14.48,
          "ozone": 290.79
        },
        "flags": {
          "sources": [
            "dwdpa",
            "cmc",
            "gfs",
            "icon",
            "isd",
            "madis"
          ],
          "nearest-station": 5.095,
          "units": "si"
        },
        "offset": 2
      }
      )json";
      REQUIRE( SimdJsonDarkSky::parseCurrentWeather(json, weather) );
      // check parsed data
      REQUIRE( weather.temperatureCelsius() == 30.49f );
      REQUIRE( weather.humidity() == 40 );
      REQUIRE( weather.rain() == 0.061f );
      REQUIRE( weather.snow() == 0.0f );
      REQUIRE( weather.pressure() == 1015 );
      REQUIRE( weather.windSpeed() == 2.8f );
      REQUIRE( weather.windDegrees() == 329 );
      REQUIRE( weather.cloudiness() == 33 );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast("", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast("    ", forecast) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast("\n", forecast) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast("\r", forecast) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast("\t\t\t\t", forecast) );
    }

    SECTION("no 'hourly' element")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset":-7
      }
      )json";
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast(json, forecast) );
    }

    SECTION("'hourly' element is not an object")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "hourly": ["hey", "not an object"],
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset":-7
      }
      )json";
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast(json, forecast) );
    }

    SECTION("no 'hourly/data' element")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "currently":
        {
          "time": 1529851404,
          "summary": "Clear",
          "icon": "clear-day",
          "nearestStormDistance": 399,
          "nearestStormBearing": 154,
          "precipIntensity": 0,
          "precipProbability": 0,
          "temperature": 15.15,
          "apparentTemperature": 15.15,
          "dewPoint": 12.33,
          "humidity": 0.83,
          "pressure": 1009.82,
          "windSpeed": 2.46,
          "windGust": 4.56,
          "windBearing": 245,
          "cloudCover": 0.14,
          "uvIndex": 1,
          "visibility": 15.98,
          "ozone": 300.84
        },
        "hourly":
        {
          "summary": "Mostly cloudy starting this evening.",
          "icon": "partly-cloudy-night"
        },
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset": -7
      }
      )json";
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast(json, forecast) );
    }

    SECTION("'hourly/data' element is not an array")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "currently":
        {
          "time": 1529851404,
          "summary": "Clear",
          "icon": "clear-day",
          "nearestStormDistance": 399,
          "nearestStormBearing": 154,
          "precipIntensity": 0,
          "precipProbability": 0,
          "temperature": 15.15,
          "apparentTemperature": 15.15,
          "dewPoint": 12.33,
          "humidity": 0.83,
          "pressure": 1009.82,
          "windSpeed": 2.46,
          "windGust": 4.56,
          "windBearing": 245,
          "cloudCover": 0.14,
          "uvIndex": 1,
          "visibility": 15.98,
          "ozone": 300.84
        },
        "hourly":
        {
          "summary": "Mostly cloudy starting this evening.",
          "icon": "partly-cloudy-night",
          "data": "This should be an array, but it is not."
        },
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset": -7
      }
      )json";
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast(json, forecast) );
    }

    SECTION("parsing of single element in 'hourly/data' fails")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "currently":
        {
          "time": 1529851404,
          "summary": "Clear",
          "icon": "clear-day",
          "nearestStormDistance": 399,
          "nearestStormBearing": 154,
          "precipIntensity": 0,
          "precipProbability": 0,
          "temperature": 15.15,
          "apparentTemperature": 15.15,
          "dewPoint": 12.33,
          "humidity": 0.83,
          "pressure": 1009.82,
          "windSpeed": 2.46,
          "windGust": 4.56,
          "windBearing": 245,
          "cloudCover": 0.14,
          "uvIndex": 1,
          "visibility": 15.98,
          "ozone": 300.84
        },
        "hourly":
        {
          "summary": "Mostly cloudy starting this evening.",
          "icon": "partly-cloudy-night",
          "data": [
            {
              "time": 1533402000,
              "summary": "This is missing basically all relevant data members.",
              "failure": true,
              "reason": "Testing"
            }
          ]
        },
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset": -7
      }
      )json";
      REQUIRE_FALSE( SimdJsonDarkSky::parseForecast(json, forecast) );
    }

    SECTION("successful parsing")
    {
      const std::string json = R"json(
      {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "currently":
        {
          "time": 1529851404,
          "summary": "Clear",
          "icon": "clear-day",
          "nearestStormDistance": 399,
          "nearestStormBearing": 154,
          "precipIntensity": 0,
          "precipProbability": 0,
          "temperature": 15.15,
          "apparentTemperature": 15.15,
          "dewPoint": 12.33,
          "humidity": 0.83,
          "pressure": 1009.82,
          "windSpeed": 2.46,
          "windGust": 4.56,
          "windBearing": 245,
          "cloudCover": 0.14,
          "uvIndex": 1,
          "visibility": 15.98,
          "ozone": 300.84
        },
        "hourly":
        {
          "summary": "Mostly cloudy starting this evening.",
          "icon": "partly-cloudy-night",
          "data": [
            {
              "time": 1533402000,
              "summary": "Partly Cloudy",
              "icon": "partly-cloudy-day",
              "precipIntensity": 0.0686,
              "precipProbability": 0.02,
              "precipType": "rain",
              "temperature": 31.18,
              "apparentTemperature": 31.18,
              "dewPoint": 14.78,
              "humidity": 0.37,
              "pressure": 1015.28,
              "windSpeed": 3.1,
              "windGust": 4.37,
              "windBearing": 320,
              "cloudCover": 0.34,
              "uvIndex": 1,
              "visibility": 12.67,
              "ozone": 290.85
            },
            {
              "time": 1533405600,
              "summary": "Partly Cloudy",
              "icon": "partly-cloudy-day",
              "precipIntensity": 0.0508,
              "precipProbability": 0.02,
              "precipType": "rain",
              "temperature": 29.51,
              "apparentTemperature": 29.51,
              "dewPoint": 15.69,
              "humidity": 0.43,
              "pressure": 1015.46,
              "windSpeed": 2.53,
              "windGust": 4.99,
              "windBearing": 345,
              "cloudCover": 0.3,
              "uvIndex": 0,
              "visibility": 16.09,
              "ozone": 290.69
            }
          ]
        },
        "flags":
        {
          "sources": ["nearest-precip","nwspa","cmc","gfs","hrrr","icon","isd","madis","nam","sref","darksky"],
          "nearest-station": 2.96,
          "units": "si"
        },
        "offset": -7
      }
      )json";
      REQUIRE( SimdJsonDarkSky::parseForecast(json, forecast) );
      // Check data.
      REQUIRE( forecast.data().size() == 2 );
      // Check first element.
      REQUIRE( forecast.data()[0].temperatureCelsius() == 31.18f );
      REQUIRE( forecast.data()[0].humidity() == 37 );
      REQUIRE( forecast.data()[0].rain() == 0.0686f );
      REQUIRE( forecast.data()[0].snow() == 0.0f );
      REQUIRE( forecast.data()[0].pressure() == 1015 );
      REQUIRE( forecast.data()[0].windSpeed() == 3.1f );
      REQUIRE( forecast.data()[0].windDegrees() == 320 );
      REQUIRE( forecast.data()[0].cloudiness() == 34 );
      // Check second element.
      REQUIRE( forecast.data()[1].temperatureCelsius() == 29.51f );
      REQUIRE( forecast.data()[1].humidity() == 43 );
      REQUIRE( forecast.data()[1].rain() == 0.0508f );
      REQUIRE( forecast.data()[1].snow() == 0.0f );
      REQUIRE( forecast.data()[1].pressure() == 1015 );
      REQUIRE( forecast.data()[1].windSpeed() == 2.53f );
      REQUIRE( forecast.data()[1].windDegrees() == 345 );
      REQUIRE( forecast.data()[1].cloudiness() == 30 );
    }
  }
}
