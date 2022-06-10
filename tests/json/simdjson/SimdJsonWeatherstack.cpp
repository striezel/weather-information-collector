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
#include "../../../src/json/SimdJsonWeatherstack.hpp"

TEST_CASE("Class SimdJsonWeatherstack")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather("", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("current element is missing")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "localtime":"2019-10-19 18:55",
              "localtime_epoch":1571511300,
              "utc_offset":"2.0"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("current element is not an object")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "localtime":"2019-10-19 18:55",
              "localtime_epoch":1571511300,
              "utc_offset":"2.0"
          },
          "current": [ ]
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("location element is missing")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("location element is not an object")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location": "I am not an object, I am a string!",
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    /*

    // Note: Format and range errors are not detected consistently.

    SECTION("localtime has wrong format")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "localtime":"2019-10-19 de:ad",
              "localtime_epoch":1571511300,
              "utc_offset":"2.0"
          },
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("localtime's date is out of range")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "localtime":"2019-17-99 18:55",
              "localtime_epoch":1571511300,
              "utc_offset":"2.0"
          },
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("localtime's time is out of range")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "localtime":"2019-10-19 18:99",
              "localtime_epoch":1571511300,
              "utc_offset":"2.0"
          },
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }
    */

    SECTION("localtime and localtime_epoch are missing")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "utc_offset":"2.0"
          },
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }

    SECTION("localtime is missing and localtime_epoch (fallback) is not an integer")
    {
      const std::string json = R"json(
      {
          "request":
          {
              "type":"City",
              "query":"Berlin, Germany",
              "language":"en",
              "unit":"m"
          },
          "location":
          {
              "name":"Berlin",
              "country":"Germany",
              "region":"Berlin",
              "lat":"52.517",
              "lon":"13.400",
              "timezone_id":"Europe\/Berlin",
              "localtime_epoch": "Not an integer",
              "utc_offset":"2.0"
          },
          "current":
          {
              "observation_time":"04:55 PM",
              "temperature":14,
              "weather_code":113,
              "weather_icons":
              [
                  "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
              ],
              "weather_descriptions":
              [
                  "Clear"
              ],
              "wind_speed":17,
              "wind_degree":150,
              "wind_dir":"SSE",
              "pressure":1009,
              "precip":0,
              "humidity":82,
              "cloudcover":0,
              "feelslike":13,
              "uv_index":0,
              "visibility":10,
              "is_day":"no"
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherstack::parseCurrentWeather(json, weather) );
    }
  }

  SECTION("parseForecast")
  {
    // Note: Forecast data retrieval is not implemented for Weatherstack.
  }
}
