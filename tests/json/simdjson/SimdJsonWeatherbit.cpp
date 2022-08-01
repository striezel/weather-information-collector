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
#include "../../../src/json/SimdJsonWeatherbit.hpp"

TEST_CASE("SimdJsonWeatherbit")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather("", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("data element is missing")
    {
      const std::string json = R"json(
      {
        "count": 1
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
    }

    SECTION("data element is not an array")
    {
      const std::string json = R"json(
      {
        "data": "I am a string, but array is expected.",
        "count": 1
      }
      )json";
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );
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
      REQUIRE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );

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

    SECTION("parsing of current weather with temperature near the 'edge'")
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
            "temp": 5.99999,
            "station": "E7277",
            "elev_angle": 41.72,
            "app_temp": 16.4
          }
        ],
        "count": 1
      }
      )json";
      REQUIRE( SimdJsonWeatherbit::parseCurrentWeather(json, weather) );

      REQUIRE( weather.temperatureCelsius() == 6.0f );
      REQUIRE( weather.temperatureKelvin() == 279.15f );
      REQUIRE( weather.temperatureFahrenheit() == 42.8f );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast("", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast("    ", forecast) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast("\n", forecast) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast("\r", forecast) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast("\t\t\t\t", forecast) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast(json, forecast) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast(json, forecast) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast(json, forecast) );
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
      REQUIRE_FALSE( SimdJsonWeatherbit::parseForecast(json, forecast) );
    }

    SECTION("successful parsing of weather forecast data")
    {
      const std::string json = R"json(
      {
        "data": [
          {
            "wind_cdir": "WSW",
            "rh": 51,
            "pod": "d",
            "timestamp_utc": "2019-03-30T16:00:00",
            "pres": 1004.34,
            "solar_rad": 195.318,
            "ozone": 344.6,
            "weather": {
              "icon": "c02d",
              "code": 801,
              "description": "Few clouds"
            },
            "wind_gust_spd": 4.88022,
            "timestamp_local": "2019-03-30T17:00:00",
            "snow_depth": 0,
            "clouds": 13,
            "ts": 1553961600,
            "wind_spd": 1.83083,
            "pop": 0,
            "wind_cdir_full": "west-southwest",
            "slp": 1020.95,
            "dni": 575.25,
            "dewpt": 6,
            "snow": 0,
            "uv": 2.03396,
            "wind_dir": 253,
            "clouds_hi": 13,
            "precip": 0,
            "vis": 24.1,
            "dhi": 63.94,
            "app_temp": 15.9,
            "datetime": "2019-03-30:16",
            "temp": 15.9,
            "ghi": 195.46,
            "clouds_mid": 0,
            "clouds_low": 0
          },
          {
            "wind_cdir": "WSW",
            "rh": 60,
            "pod": "n",
            "timestamp_utc": "2019-03-30T18:00:00",
            "pres": 1005.25,
            "solar_rad": 0,
            "ozone": 345.811,
            "weather": {
              "icon": "c01n",
              "code": 800,
              "description": "Clear sky"
            },
            "wind_gust_spd": 3.09386,
            "timestamp_local": "2019-03-30T19:00:00",
            "snow_depth": 0,
            "clouds": 1,
            "ts": 1553968800,
            "wind_spd": 0.959623,
            "pop": 0,
            "wind_cdir_full": "west-southwest",
            "slp": 1020.99,
            "dni": 0,
            "dewpt": 5.5,
            "snow": 0,
            "uv": 0,
            "wind_dir": 240,
            "clouds_hi": 1,
            "precip": 0,
            "vis": 24.1,
            "dhi": 0,
            "app_temp": 12.8,
            "datetime": "2019-03-30:18",
            "temp": 12.8,
            "ghi": 0,
            "clouds_mid": 0,
            "clouds_low": 0
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
      REQUIRE( SimdJsonWeatherbit::parseForecast(json, forecast) );

      REQUIRE( forecast.data().size() == 2 );

      REQUIRE( forecast.data().at(0).temperatureCelsius() == 15.9f );
      REQUIRE( forecast.data().at(0).temperatureKelvin() == 289.05f );
      REQUIRE( forecast.data().at(0).temperatureFahrenheit() == 60.62f );
      REQUIRE( forecast.data().at(0).humidity() == 51 );
      REQUIRE( forecast.data().at(0).rain() == 0.0f );
      REQUIRE( forecast.data().at(0).snow() == 0.0f );
      REQUIRE( forecast.data().at(0).pressure() == 1004 );
      REQUIRE( forecast.data().at(0).windSpeed() == 1.83083f );
      REQUIRE( forecast.data().at(0).windDegrees() == 253 );
      REQUIRE( forecast.data().at(0).cloudiness() == 13 );

      REQUIRE( forecast.data().at(1).temperatureCelsius() == 12.8f );
      REQUIRE( forecast.data().at(1).temperatureKelvin() == 285.95f );
      REQUIRE( forecast.data().at(1).temperatureFahrenheit() == 55.04f );
      REQUIRE( forecast.data().at(1).humidity() == 60 );
      REQUIRE( forecast.data().at(1).rain() == 0.0f );
      REQUIRE( forecast.data().at(1).snow() == 0.0f );
      REQUIRE( forecast.data().at(1).pressure() == 1005 );
      REQUIRE( forecast.data().at(1).windSpeed() == 0.959623f );
      REQUIRE( forecast.data().at(1).windDegrees() == 240 );
      REQUIRE( forecast.data().at(1).cloudiness() == 1 );
    }
  }
}
