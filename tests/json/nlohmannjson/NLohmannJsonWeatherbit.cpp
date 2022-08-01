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
#include "../../../src/json/NLohmannJsonWeatherbit.hpp"

TEST_CASE("NLohmannJsonWeatherbit")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather("", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("data element is missing")
    {
      const std::string json = R"json(
      {
        "count": 1
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("data element is not an array")
    {
      const std::string json = R"json(
      {
        "data": "I am a string, but array is expected.",
        "count": 1
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("count element is missing")
    {
      const std::string json = R"json(
      {
        "data": [
          {
            "rh": 42,
            "pod": "d",
            "lon": 13.73832,
            "pres": 1005.65,
            "timezone": "Europe/Berlin",
            "ob_time": "2019-03-30 12:37",
            "country_code": "DE",
            "clouds": 72,
            "ts": 1553949441.8558,
            "solar_rad": 459.262,
            "state_code": "13",
            "lat": 51.05089,
            "wind_spd": 2.00069,
            "wind_cdir_full": "west",
            "wind_cdir": "W",
            "slp": 1022.28,
            "vis": 24.1,
            "h_angle": 12.9,
            "sunset": "17:32",
            "dni": 840.12,
            "dewpt": 3.8,
            "snow": 0,
            "uv": 2.26583,
            "precip": 0,
            "wind_dir": 264,
            "sunrise": "04:43",
            "ghi": 608.67,
            "dhi": 103.26,
            "aqi": 33,
            "city_name": "Dresden",
            "weather": {
              "icon": "c04d",
              "code": 804,
              "description": "Overcast clouds"
            },
            "datetime": "2019-03-30:12",
            "temp": 16.4,
            "station": "E7277",
            "elev_angle": 41.72,
            "app_temp": 16.4
          }
        ]
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("count element is not an integer")
    {
      const std::string json = R"json(
      {
        "data": [
          {
            "rh": 42,
            "pod": "d",
            "lon": 13.73832,
            "pres": 1005.65,
            "timezone": "Europe/Berlin",
            "ob_time": "2019-03-30 12:37",
            "country_code": "DE",
            "clouds": 72,
            "ts": 1553949441.8558,
            "solar_rad": 459.262,
            "state_code": "13",
            "lat": 51.05089,
            "wind_spd": 2.00069,
            "wind_cdir_full": "west",
            "wind_cdir": "W",
            "slp": 1022.28,
            "vis": 24.1,
            "h_angle": 12.9,
            "sunset": "17:32",
            "dni": 840.12,
            "dewpt": 3.8,
            "snow": 0,
            "uv": 2.26583,
            "precip": 0,
            "wind_dir": 264,
            "sunrise": "04:43",
            "ghi": 608.67,
            "dhi": 103.26,
            "aqi": 33,
            "city_name": "Dresden",
            "weather": {
              "icon": "c04d",
              "code": 804,
              "description": "Overcast clouds"
            },
            "datetime": "2019-03-30:12",
            "temp": 16.4,
            "station": "E7277",
            "elev_angle": 41.72,
            "app_temp": 16.4
          }
        ],
        "count": {}
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("count element is an integer, but it is not equal to one")
    {
      const std::string json = R"json(
      {
        "data": [
          {
            "rh": 42,
            "pod": "d",
            "lon": 13.73832,
            "pres": 1005.65,
            "timezone": "Europe/Berlin",
            "ob_time": "2019-03-30 12:37",
            "country_code": "DE",
            "clouds": 72,
            "ts": 1553949441.8558,
            "solar_rad": 459.262,
            "state_code": "13",
            "lat": 51.05089,
            "wind_spd": 2.00069,
            "wind_cdir_full": "west",
            "wind_cdir": "W",
            "slp": 1022.28,
            "vis": 24.1,
            "h_angle": 12.9,
            "sunset": "17:32",
            "dni": 840.12,
            "dewpt": 3.8,
            "snow": 0,
            "uv": 2.26583,
            "precip": 0,
            "wind_dir": 264,
            "sunrise": "04:43",
            "ghi": 608.67,
            "dhi": 103.26,
            "aqi": 33,
            "city_name": "Dresden",
            "weather": {
              "icon": "c04d",
              "code": 804,
              "description": "Overcast clouds"
            },
            "datetime": "2019-03-30:12",
            "temp": 16.4,
            "station": "E7277",
            "elev_angle": 41.72,
            "app_temp": 16.4
          }
        ],
        "count": 2
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("element of data array is not an object")
    {
      const std::string json = R"json(
      {
        "data": [
          42
        ],
        "count": 1
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("successful parsing of current weather")
    {
      const std::string json = R"json(
      {
        "data": [
          {
            "rh": 42,
            "pod": "d",
            "lon": 13.73832,
            "pres": 1005.65,
            "timezone": "Europe/Berlin",
            "ob_time": "2019-03-30 12:37",
            "country_code": "DE",
            "clouds": 72,
            "ts": 1553949441.8558,
            "solar_rad": 459.262,
            "state_code": "13",
            "lat": 51.05089,
            "wind_spd": 2.00069,
            "wind_cdir_full": "west",
            "wind_cdir": "W",
            "slp": 1022.28,
            "vis": 24.1,
            "h_angle": 12.9,
            "sunset": "17:32",
            "dni": 840.12,
            "dewpt": 3.8,
            "snow": 4.25,
            "uv": 2.26583,
            "precip": 0.5,
            "wind_dir": 264,
            "sunrise": "04:43",
            "ghi": 608.67,
            "dhi": 103.26,
            "aqi": 33,
            "city_name": "Dresden",
            "weather": {
              "icon": "c04d",
              "code": 804,
              "description": "Overcast clouds"
            },
            "datetime": "2019-03-30:12",
            "temp": 16.4,
            "station": "E7277",
            "elev_angle": 41.72,
            "app_temp": 16.4
          }
        ],
        "count": 1
      }
      )json";
      REQUIRE( NLohmannJsonWeatherbit::parseCurrentWeather(json, weather) );

      REQUIRE( weather.temperatureCelsius() == 16.4f );
      REQUIRE( weather.temperatureKelvin() == 289.55f );
      REQUIRE( weather.temperatureFahrenheit() == 61.52f );
      REQUIRE( weather.humidity() == 42 );
      REQUIRE( weather.rain() == 0.5f );
      // Snow amount is fabricated for this test, real value was zero, because
      // there is no snow at that temperature.
      REQUIRE( weather.snow() == 4.25f );
      REQUIRE( weather.pressure() == 1005 );
      REQUIRE( weather.windSpeed() == 2.00069f );
      REQUIRE( weather.windDegrees() == 264 );
      REQUIRE( weather.cloudiness() == 72 );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast("", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast("    ", forecast) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast("\n", forecast) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast("\r", forecast) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast("\t\t\t\t", forecast) );
    }

    SECTION("data element is missing")
    {
      const std::string json = R"json(
      {
          "city_name": "Dresden",
          "lon": "13.73832",
          "timezone": "Europe/Berlin",
          "lat": "51.05089",
          "country_code": "DE",
          "state_code": "13"
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast(json, forecast) );
    }

    SECTION("data element is not an array")
    {
      const std::string json = R"json(
      {
          "data": "String is bad, array would be good here.",
          "city_name": "Dresden",
          "lon": "13.73832",
          "timezone": "Europe/Berlin",
          "lat": "51.05089",
          "country_code": "DE",
          "state_code": "13"
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast(json, forecast) );
    }

    SECTION("data array does not contain valid data")
    {
      const std::string json = R"json(
      {
          "data": [
              {
                 "this is unexpected": 29
              }
          ],
          "city_name": "Dresden",
          "lon": "13.73832",
          "timezone": "Europe/Berlin",
          "lat": "51.05089",
          "country_code": "DE",
          "state_code": "13"
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast(json, forecast) );
    }

    SECTION("data array is empty")
    {
      const std::string json = R"json(
      {
          "data": [],
          "city_name": "Dresden",
          "lon": "13.73832",
          "timezone": "Europe/Berlin",
          "lat": "51.05089",
          "country_code": "DE",
          "state_code": "13"
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonWeatherbit::parseForecast(json, forecast) );
    }
  }
}
