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
#include "../../../src/json/NLohmannJsonApixu.hpp"

TEST_CASE("NLohmannJsonApixu")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather("{ }", weather) );
    }

    SECTION("current element is missing")
    {
      const std::string json = R"json(
      {
        "location": {
          "name": "Dresden",
          "region": "Sachsen",
          "country": "Germany",
          "lat": 51.05,
          "lon": 13.75,
          "tz_id": "Europe/Berlin",
          "localtime_epoch": 1502861793,
          "localtime": "2017-08-16 7:36"
        }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather(json, weather) );
    }

    SECTION("current element is not an object")
    {
      const std::string json = R"json(
      {
        "location": {
          "name": "Dresden",
          "region": "Sachsen",
          "country": "Germany",
          "lat": 51.05,
          "lon": 13.75,
          "tz_id": "Europe/Berlin",
          "localtime_epoch": 1502861793,
          "localtime": "2017-08-16 7:36"
        },
        "current": "I am not an object but a string."
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseCurrentWeather(json, weather) );
    }

    SECTION("successful parsing")
    {
      const std::string json = R"json(
      {
          "location":{
              "name":"Polyarka",
              "region":"Sakha",
              "country":"Russia",
              "lat":71.64,
              "lon":128.87,
              "tz_id":"Asia/Yakutsk",
              "localtime_epoch":1545697740,
              "localtime":"2018-12-25 9:29"
          },
          "current":{
              "last_updated_epoch":1545696604,
              "last_updated":"2018-12-25 09:10",
              "temp_c":-28.0,
              "temp_f":-18.4,
              "is_day":0,
              "condition":{
                  "text":"Blowing snow",
                  "icon":"//cdn.apixu.com/weather/64x64/night/227.png",
                  "code":1114
              },
              "wind_mph":24.6,
              "wind_kph":39.6,
              "wind_degree":243,
              "wind_dir":"WSW",
              "pressure_mb":1021.0,
              "pressure_in":30.6,
              "precip_mm":0.1,
              "precip_in":0.0,
              "humidity":92,
              "cloud":100,
              "feelslike_c":-44.8,
              "feelslike_f":-48.6,
              "vis_km":14.5,
              "vis_miles":9.0,
              "uv":0.0
          }
      }
      )json";
      REQUIRE( NLohmannJsonApixu::parseCurrentWeather(json, weather) );

      REQUIRE( weather.temperatureCelsius() == -28.0f );
      REQUIRE( weather.temperatureFahrenheit() == -18.4f );
      REQUIRE( weather.windSpeed() == 11.0f );
      REQUIRE( weather.windDegrees() == 243 );
      REQUIRE( weather.humidity() == 92 );
      REQUIRE( weather.cloudiness() == 100 );
      REQUIRE( weather.pressure() == 1021 );
      REQUIRE( weather.snow() == 0.1f );
      REQUIRE( weather.rain() == 0.0f );
    }

    SECTION("successful parsing without wind_kph uses wind_mph")
    {
      const std::string json = R"json(
      {
          "location":{
              "name":"Polyarka",
              "region":"Sakha",
              "country":"Russia",
              "lat":71.64,
              "lon":128.87,
              "tz_id":"Asia/Yakutsk",
              "localtime_epoch":1545697740,
              "localtime":"2018-12-25 9:29"
          },
          "current":{
              "last_updated_epoch":1545696604,
              "last_updated":"2018-12-25 09:10",
              "temp_c":-28.0,
              "temp_f":-18.4,
              "is_day":0,
              "condition":{
                  "text":"Blowing snow",
                  "icon":"//cdn.apixu.com/weather/64x64/night/227.png",
                  "code":1114
              },
              "wind_mph":24.6,
              "wind_degree":243,
              "wind_dir":"WSW",
              "pressure_mb":1021.0,
              "pressure_in":30.6,
              "precip_mm":0.1,
              "precip_in":0.0,
              "humidity":92,
              "cloud":100,
              "feelslike_c":-44.8,
              "feelslike_f":-48.6,
              "vis_km":14.5,
              "vis_miles":9.0,
              "uv":0.0
          }
      }
      )json";
      REQUIRE( NLohmannJsonApixu::parseCurrentWeather(json, weather) );

      REQUIRE( weather.temperatureCelsius() == -28.0f );
      REQUIRE( weather.temperatureFahrenheit() == -18.4f );
      REQUIRE( weather.windSpeed() == 10.9971840f );
      REQUIRE( weather.windDegrees() == 243 );
      REQUIRE( weather.humidity() == 92 );
      REQUIRE( weather.cloudiness() == 100 );
      REQUIRE( weather.pressure() == 1021 );
      REQUIRE( weather.snow() == 0.1f );
      REQUIRE( weather.rain() == 0.0f );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("    ", forecast) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("\n", forecast) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("\r", forecast) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("\t\t\t\t", forecast) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast("{ }", forecast) );
    }

    SECTION("forecast element is missing")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("forecast element is not an object")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": 123.4
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("forecastday element is missing")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("forecastday element is not an array")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
              "forecastday": "You get no array. Good day, sir!"
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("date_epoch element is missing")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
              "forecastday": [
                  {
                      "date": "2018-07-31",
                      "day": {
                          "maxtemp_c": 26,
                          "maxtemp_f": 78.8,
                          "mintemp_c": 17,
                          "mintemp_f": 62.6,
                          "avgtemp_c": 22.2,
                          "avgtemp_f": 72,
                          "maxwind_mph": 5.6,
                          "maxwind_kph": 9,
                          "totalprecip_mm": 0.1,
                          "totalprecip_in": 0,
                          "avgvis_km": 10,
                          "avgvis_miles": 6,
                          "avghumidity": 53,
                          "condition": {
                              "text": "Patchy rain possible",
                              "icon": "//cdn.apixu.com/weather/64x64/day/176.png",
                              "code": 1063
                          },
                          "uv": 6.6
                      },
                      "astro": {
                          "sunrise": "06:22 AM",
                          "sunset": "09:31 PM",
                          "moonrise": "11:24 PM",
                          "moonset": "09:49 AM"
                      }
                  }
              ]
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("date_epoch element is not an integer")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
              "forecastday": [
                  {
                      "date": "2018-07-31",
                      "date_epoch": "This is not an integer value.",
                      "day": {
                          "maxtemp_c": 26,
                          "maxtemp_f": 78.8,
                          "mintemp_c": 17,
                          "mintemp_f": 62.6,
                          "avgtemp_c": 22.2,
                          "avgtemp_f": 72,
                          "maxwind_mph": 5.6,
                          "maxwind_kph": 9,
                          "totalprecip_mm": 0.1,
                          "totalprecip_in": 0,
                          "avgvis_km": 10,
                          "avgvis_miles": 6,
                          "avghumidity": 53,
                          "condition": {
                              "text": "Patchy rain possible",
                              "icon": "//cdn.apixu.com/weather/64x64/day/176.png",
                              "code": 1063
                          },
                          "uv": 6.6
                      },
                      "astro": {
                          "sunrise": "06:22 AM",
                          "sunset": "09:31 PM",
                          "moonrise": "11:24 PM",
                          "moonset": "09:49 AM"
                      }
                  }
              ]
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("day element is missing")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
              "forecastday": [
                  {
                      "date": "2018-07-31",
                      "date_epoch": 1532995200,
                      "astro": {
                          "sunrise": "06:22 AM",
                          "sunset": "09:31 PM",
                          "moonrise": "11:24 PM",
                          "moonset": "09:49 AM"
                      }
                  }
              ]
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("day element is not an object")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
              "forecastday": [
                  {
                      "date": "2018-07-31",
                      "date_epoch": 1532995200,
                      "day": [ ],
                      "astro": {
                          "sunrise": "06:22 AM",
                          "sunset": "09:31 PM",
                          "moonrise": "11:24 PM",
                          "moonset": "09:49 AM"
                      }
                  }
              ]
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("hour element is present but not an array")
    {
      const std::string json = R"json(
      {
        "location": {
          "name": "London",
          "region": "City of London, Greater London",
          "country": "United Kingdom",
          "lat": 51.52,
          "lon": -0.11,
          "tz_id": "Europe/London",
          "localtime_epoch": 1520074806,
          "localtime": "2018-03-03 11:00"
        },
        "current": {
          "last_updated_epoch": 1520073933,
          "last_updated": "2018-03-03 10:45",
          "temp_c": 2.0,
          "temp_f": 35.6,
          "is_day": 1,
          "condition": {
            "text": "Mist",
            "icon": "//cdn.apixu.com/weather/64x64/day/143.png",
            "code": 1030
          },
          "wind_mph": 16.1,
          "wind_kph": 25.9,
          "wind_degree": 90,
          "wind_dir": "E",
          "pressure_mb": 993.0,
          "pressure_in": 29.8,
          "precip_mm": 0.1,
          "precip_in": 0.0,
          "humidity": 93,
          "cloud": 100,
          "feelslike_c": -3.4,
          "feelslike_f": 25.8,
          "vis_km": 2.0,
          "vis_miles": 1.0
        },
        "forecast": {
          "forecastday": [
            {
              "date": "2018-03-03",
              "date_epoch": 1520035200,
              "day": {
                "maxtemp_c": 4.1,
                "maxtemp_f": 39.4,
                "mintemp_c": 2.5,
                "mintemp_f": 36.5,
                "avgtemp_c": 2.5,
                "avgtemp_f": 36.4,
                "maxwind_mph": 8.5,
                "maxwind_kph": 13.7,
                "totalprecip_mm": 1.2,
                "totalprecip_in": 0.05,
                "avgvis_km": 13.2,
                "avgvis_miles": 8.0,
                "avghumidity": 95.0,
                "condition": {
                  "text": "Fog",
                  "icon": "//cdn.apixu.com/weather/64x64/day/248.png",
                  "code": 1135
                },
                "uv": 1.2
              },
              "astro": {
                "sunrise": "06:41 AM",
                "sunset": "05:44 PM",
                "moonrise": "07:40 PM",
                "moonset": "07:38 AM"
              },
              "hour": "This is not an array, but it should be."
            }
          ]
        }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("time_epoch element is missing")
    {
      const std::string json = R"json(
      {
        "location": {
          "name": "London",
          "region": "City of London, Greater London",
          "country": "United Kingdom",
          "lat": 51.52,
          "lon": -0.11,
          "tz_id": "Europe/London",
          "localtime_epoch": 1520074806,
          "localtime": "2018-03-03 11:00"
        },
        "current": {
          "last_updated_epoch": 1520073933,
          "last_updated": "2018-03-03 10:45",
          "temp_c": 2.0,
          "temp_f": 35.6,
          "is_day": 1,
          "condition": {
            "text": "Mist",
            "icon": "//cdn.apixu.com/weather/64x64/day/143.png",
            "code": 1030
          },
          "wind_mph": 16.1,
          "wind_kph": 25.9,
          "wind_degree": 90,
          "wind_dir": "E",
          "pressure_mb": 993.0,
          "pressure_in": 29.8,
          "precip_mm": 0.1,
          "precip_in": 0.0,
          "humidity": 93,
          "cloud": 100,
          "feelslike_c": -3.4,
          "feelslike_f": 25.8,
          "vis_km": 2.0,
          "vis_miles": 1.0
        },
        "forecast": {
          "forecastday": [
            {
              "date": "2018-03-03",
              "date_epoch": 1520035200,
              "day": {
                "maxtemp_c": 4.1,
                "maxtemp_f": 39.4,
                "mintemp_c": 2.5,
                "mintemp_f": 36.5,
                "avgtemp_c": 2.5,
                "avgtemp_f": 36.4,
                "maxwind_mph": 8.5,
                "maxwind_kph": 13.7,
                "totalprecip_mm": 1.2,
                "totalprecip_in": 0.05,
                "avgvis_km": 13.2,
                "avgvis_miles": 8.0,
                "avghumidity": 95.0,
                "condition": {
                  "text": "Fog",
                  "icon": "//cdn.apixu.com/weather/64x64/day/248.png",
                  "code": 1135
                },
                "uv": 1.2
              },
              "astro": {
                "sunrise": "06:41 AM",
                "sunset": "05:44 PM",
                "moonrise": "07:40 PM",
                "moonset": "07:38 AM"
              },
              "hour": [
                {
                  "time": "2018-03-03 00:00",
                  "temp_c": 1.2,
                  "temp_f": 34.2,
                  "is_day": 0,
                  "condition": {
                    "text": "Light drizzle",
                    "icon": "//cdn.apixu.com/weather/64x64/night/266.png",
                    "code": 1153
                  },
                  "wind_mph": 8.3,
                  "wind_kph": 13.3,
                  "wind_degree": 86,
                  "wind_dir": "E",
                  "pressure_mb": 993.0,
                  "pressure_in": 29.8,
                  "precip_mm": 0.6,
                  "precip_in": 0.02,
                  "humidity": 98,
                  "cloud": 100,
                  "feelslike_c": -2.6,
                  "feelslike_f": 27.3,
                  "windchill_c": -2.6,
                  "windchill_f": 27.3,
                  "heatindex_c": 1.2,
                  "heatindex_f": 34.2,
                  "dewpoint_c": 1.0,
                  "dewpoint_f": 33.8,
                  "will_it_rain": 1,
                  "chance_of_rain": "94",
                  "will_it_snow": 0,
                  "chance_of_snow": "3",
                  "vis_km": 10.2,
                  "vis_miles": 6.0
                }
              ]
            }
          ]
        }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("time_epoch element is not an integer")
    {
      const std::string json = R"json(
      {
        "location": {
          "name": "London",
          "region": "City of London, Greater London",
          "country": "United Kingdom",
          "lat": 51.52,
          "lon": -0.11,
          "tz_id": "Europe/London",
          "localtime_epoch": 1520074806,
          "localtime": "2018-03-03 11:00"
        },
        "current": {
          "last_updated_epoch": 1520073933,
          "last_updated": "2018-03-03 10:45",
          "temp_c": 2.0,
          "temp_f": 35.6,
          "is_day": 1,
          "condition": {
            "text": "Mist",
            "icon": "//cdn.apixu.com/weather/64x64/day/143.png",
            "code": 1030
          },
          "wind_mph": 16.1,
          "wind_kph": 25.9,
          "wind_degree": 90,
          "wind_dir": "E",
          "pressure_mb": 993.0,
          "pressure_in": 29.8,
          "precip_mm": 0.1,
          "precip_in": 0.0,
          "humidity": 93,
          "cloud": 100,
          "feelslike_c": -3.4,
          "feelslike_f": 25.8,
          "vis_km": 2.0,
          "vis_miles": 1.0
        },
        "forecast": {
          "forecastday": [
            {
              "date": "2018-03-03",
              "date_epoch": 1520035200,
              "day": {
                "maxtemp_c": 4.1,
                "maxtemp_f": 39.4,
                "mintemp_c": 2.5,
                "mintemp_f": 36.5,
                "avgtemp_c": 2.5,
                "avgtemp_f": 36.4,
                "maxwind_mph": 8.5,
                "maxwind_kph": 13.7,
                "totalprecip_mm": 1.2,
                "totalprecip_in": 0.05,
                "avgvis_km": 13.2,
                "avgvis_miles": 8.0,
                "avghumidity": 95.0,
                "condition": {
                  "text": "Fog",
                  "icon": "//cdn.apixu.com/weather/64x64/day/248.png",
                  "code": 1135
                },
                "uv": 1.2
              },
              "astro": {
                "sunrise": "06:41 AM",
                "sunset": "05:44 PM",
                "moonrise": "07:40 PM",
                "moonset": "07:38 AM"
              },
              "hour": [
                {
                  "time_epoch": true,
                  "time": "2018-03-03 00:00",
                  "temp_c": 1.2,
                  "temp_f": 34.2,
                  "is_day": 0,
                  "condition": {
                    "text": "Light drizzle",
                    "icon": "//cdn.apixu.com/weather/64x64/night/266.png",
                    "code": 1153
                  },
                  "wind_mph": 8.3,
                  "wind_kph": 13.3,
                  "wind_degree": 86,
                  "wind_dir": "E",
                  "pressure_mb": 993.0,
                  "pressure_in": 29.8,
                  "precip_mm": 0.6,
                  "precip_in": 0.02,
                  "humidity": 98,
                  "cloud": 100,
                  "feelslike_c": -2.6,
                  "feelslike_f": 27.3,
                  "windchill_c": -2.6,
                  "windchill_f": 27.3,
                  "heatindex_c": 1.2,
                  "heatindex_f": 34.2,
                  "dewpoint_c": 1.0,
                  "dewpoint_f": 33.8,
                  "will_it_rain": 1,
                  "chance_of_rain": "94",
                  "will_it_snow": 0,
                  "chance_of_snow": "3",
                  "vis_km": 10.2,
                  "vis_miles": 6.0
                }
              ]
            }
          ]
        }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("JSON with no forecast data")
    {
      const std::string json = R"json(
      {
          "location": {
              "name": "Paris",
              "region": "Ile-de-France",
              "country": "France",
              "lat": 48.87,
              "lon": 2.33,
              "tz_id": "Europe/Paris",
              "localtime_epoch": 1533055714,
              "localtime": "2018-07-31 18:48"
          },
          "current": {
              "last_updated_epoch": 1533054609,
              "last_updated": "2018-07-31 18:30",
              "temp_c": 28,
              "temp_f": 82.4,
              "is_day": 1,
              "condition": {
                  "text": "Sunny",
                  "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                  "code": 1000
              },
              "wind_mph": 12.5,
              "wind_kph": 20.2,
              "wind_degree": 250,
              "wind_dir": "WSW",
              "pressure_mb": 1018,
              "pressure_in": 30.5,
              "precip_mm": 0,
              "precip_in": 0,
              "humidity": 42,
              "cloud": 0,
              "feelslike_c": 28,
              "feelslike_f": 82.4,
              "vis_km": 10,
              "vis_miles": 6
          },
          "forecast": {
              "forecastday": [
              ]
          }
      }
      )json";
      REQUIRE_FALSE( NLohmannJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("successful forecast parsing with daily forecast data")
    {
      const std::string json = R"json(
      {
        "location": {
            "name": "Paris",
            "region": "Ile-de-France",
            "country": "France",
            "lat": 48.87,
            "lon": 2.33,
            "tz_id": "Europe/Paris",
            "localtime_epoch": 1533055714,
            "localtime": "2018-07-31 18:48"
        },
        "current": {
            "last_updated_epoch": 1533054609,
            "last_updated": "2018-07-31 18:30",
            "temp_c": 28,
            "temp_f": 82.4,
            "is_day": 1,
            "condition": {
                "text": "Sunny",
                "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                "code": 1000
            },
            "wind_mph": 12.5,
            "wind_kph": 20.2,
            "wind_degree": 250,
            "wind_dir": "WSW",
            "pressure_mb": 1018,
            "pressure_in": 30.5,
            "precip_mm": 0,
            "precip_in": 0,
            "humidity": 42,
            "cloud": 0,
            "feelslike_c": 28,
            "feelslike_f": 82.4,
            "vis_km": 10,
            "vis_miles": 6
        },
        "forecast": {
            "forecastday": [
                {
                    "date": "2018-07-31",
                    "date_epoch": 1532995200,
                    "day": {
                        "maxtemp_c": 26,
                        "maxtemp_f": 78.8,
                        "mintemp_c": 17,
                        "mintemp_f": 62.6,
                        "avgtemp_c": 22.2,
                        "avgtemp_f": 72,
                        "maxwind_mph": 5.6,
                        "maxwind_kph": 9,
                        "totalprecip_mm": 0.1,
                        "totalprecip_in": 0,
                        "avgvis_km": 10,
                        "avgvis_miles": 6,
                        "avghumidity": 53,
                        "condition": {
                            "text": "Patchy rain possible",
                            "icon": "//cdn.apixu.com/weather/64x64/day/176.png",
                            "code": 1063
                        },
                        "uv": 6.6
                    },
                    "astro": {
                        "sunrise": "06:22 AM",
                        "sunset": "09:31 PM",
                        "moonrise": "11:24 PM",
                        "moonset": "09:49 AM"
                    }
                },
                {
                    "date": "2018-08-01",
                    "date_epoch": 1533081600,
                    "day": {
                        "maxtemp_c": 29.2,
                        "maxtemp_f": 84.6,
                        "mintemp_c": 22.7,
                        "mintemp_f": 72.9,
                        "avgtemp_c": 23.9,
                        "avgtemp_f": 75,
                        "maxwind_mph": 6.9,
                        "maxwind_kph": 11.2,
                        "totalprecip_mm": 0,
                        "totalprecip_in": 0,
                        "avgvis_km": 10,
                        "avgvis_miles": 6,
                        "avghumidity": 52,
                        "condition": {
                            "text": "Partly cloudy",
                            "icon": "//cdn.apixu.com/weather/64x64/day/116.png",
                            "code": 1003
                        },
                        "uv": 7.2
                    },
                    "astro": {
                        "sunrise": "06:23 AM",
                        "sunset": "09:30 PM",
                        "moonrise": "11:47 PM",
                        "moonset": "10:53 AM"
                    }
                },
                {
                    "date": "2018-08-02",
                    "date_epoch": 1533168000,
                    "day": {
                        "maxtemp_c": 32.5,
                        "maxtemp_f": 90.5,
                        "mintemp_c": 22.2,
                        "mintemp_f": 72,
                        "avgtemp_c": 27.1,
                        "avgtemp_f": 80.8,
                        "maxwind_mph": 8.5,
                        "maxwind_kph": 13.7,
                        "totalprecip_mm": 0,
                        "totalprecip_in": 0,
                        "avgvis_km": 10,
                        "avgvis_miles": 6,
                        "avghumidity": 47,
                        "condition": {
                            "text": "Sunny",
                            "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                            "code": 1000
                        },
                        "uv": 7.8
                    },
                    "astro": {
                        "sunrise": "06:25 AM",
                        "sunset": "09:28 PM",
                        "moonrise": "No moonrise",
                        "moonset": "11:59 AM"
                    }
                }
            ]
          }
      }
      )json";
      REQUIRE( NLohmannJsonApixu::parseForecast(json, forecast) );

      REQUIRE( forecast.data().size() == 6 );

      REQUIRE( forecast.data()[0].temperatureCelsius() == 17.0f );
      REQUIRE( forecast.data()[0].temperatureFahrenheit() == 62.6f );
      REQUIRE( forecast.data()[0].humidity() == 53 );
      REQUIRE( forecast.data()[0].rain() == 0.1f );
      REQUIRE( forecast.data()[0].snow() == 0.0f );

      REQUIRE( forecast.data()[1].temperatureCelsius() == 26.0f );
      REQUIRE( forecast.data()[1].temperatureFahrenheit() == 78.8f );
      REQUIRE( forecast.data()[1].humidity() == 53 );
      REQUIRE( forecast.data()[1].rain() == 0.1f );
      REQUIRE( forecast.data()[1].snow() == 0.0f );

      REQUIRE( forecast.data()[2].temperatureCelsius() == 22.7f );
      REQUIRE( forecast.data()[2].temperatureFahrenheit() == 72.9f );
      REQUIRE( forecast.data()[2].humidity() == 52 );
      REQUIRE( forecast.data()[2].rain() == 0.0f );
      REQUIRE( forecast.data()[2].snow() == 0.0f );

      REQUIRE( forecast.data()[3].temperatureCelsius() == 29.2f );
      REQUIRE( forecast.data()[3].temperatureFahrenheit() == 84.6f );
      REQUIRE( forecast.data()[3].humidity() == 52 );
      REQUIRE( forecast.data()[3].rain() == 0.0f );
      REQUIRE( forecast.data()[3].snow() == 0.0f );

      REQUIRE( forecast.data()[4].temperatureCelsius() == 22.2f );
      REQUIRE( forecast.data()[4].temperatureFahrenheit() == 72.0f );
      REQUIRE( forecast.data()[4].humidity() == 47 );
      REQUIRE( forecast.data()[4].rain() == 0.0f );
      REQUIRE( forecast.data()[4].snow() == 0.0f );

      REQUIRE( forecast.data()[5].temperatureCelsius() == 32.5f );
      REQUIRE( forecast.data()[5].temperatureFahrenheit() == 90.5f );
      REQUIRE( forecast.data()[5].humidity() == 47 );
      REQUIRE( forecast.data()[5].rain() == 0.0f );
      REQUIRE( forecast.data()[5].snow() == 0.0f );
    }

    SECTION("successful forecast parsing with hourly forecast data")
    {
      const std::string json = R"json(
      {
        "location": {
          "name": "London",
          "region": "City of London, Greater London",
          "country": "United Kingdom",
          "lat": 51.52,
          "lon": -0.11,
          "tz_id": "Europe/London",
          "localtime_epoch": 1520074806,
          "localtime": "2018-03-03 11:00"
        },
        "current": {
          "last_updated_epoch": 1520073933,
          "last_updated": "2018-03-03 10:45",
          "temp_c": 2.0,
          "temp_f": 35.6,
          "is_day": 1,
          "condition": {
            "text": "Mist",
            "icon": "//cdn.apixu.com/weather/64x64/day/143.png",
            "code": 1030
          },
          "wind_mph": 16.1,
          "wind_kph": 25.9,
          "wind_degree": 90,
          "wind_dir": "E",
          "pressure_mb": 993.0,
          "pressure_in": 29.8,
          "precip_mm": 0.1,
          "precip_in": 0.0,
          "humidity": 93,
          "cloud": 100,
          "feelslike_c": -3.4,
          "feelslike_f": 25.8,
          "vis_km": 2.0,
          "vis_miles": 1.0
        },
        "forecast": {
          "forecastday": [
            {
              "date": "2018-03-03",
              "date_epoch": 1520035200,
              "day": {
                "maxtemp_c": 4.1,
                "maxtemp_f": 39.4,
                "mintemp_c": 2.5,
                "mintemp_f": 36.5,
                "avgtemp_c": 2.5,
                "avgtemp_f": 36.4,
                "maxwind_mph": 8.5,
                "maxwind_kph": 13.7,
                "totalprecip_mm": 1.2,
                "totalprecip_in": 0.05,
                "avgvis_km": 13.2,
                "avgvis_miles": 8.0,
                "avghumidity": 95.0,
                "condition": {
                  "text": "Fog",
                  "icon": "//cdn.apixu.com/weather/64x64/day/248.png",
                  "code": 1135
                },
                "uv": 1.2
              },
              "astro": {
                "sunrise": "06:41 AM",
                "sunset": "05:44 PM",
                "moonrise": "07:40 PM",
                "moonset": "07:38 AM"
              },
              "hour": [
                {
                  "time_epoch": 1520035200,
                  "time": "2018-03-03 00:00",
                  "temp_c": 1.2,
                  "temp_f": 34.2,
                  "is_day": 0,
                  "condition": {
                    "text": "Light drizzle",
                    "icon": "//cdn.apixu.com/weather/64x64/night/266.png",
                    "code": 1153
                  },
                  "wind_mph": 8.3,
                  "wind_kph": 13.3,
                  "wind_degree": 86,
                  "wind_dir": "E",
                  "pressure_mb": 993.0,
                  "pressure_in": 29.8,
                  "precip_mm": 0.6,
                  "precip_in": 0.02,
                  "humidity": 98,
                  "cloud": 100,
                  "feelslike_c": -2.6,
                  "feelslike_f": 27.3,
                  "windchill_c": -2.6,
                  "windchill_f": 27.3,
                  "heatindex_c": 1.2,
                  "heatindex_f": 34.2,
                  "dewpoint_c": 1.0,
                  "dewpoint_f": 33.8,
                  "will_it_rain": 1,
                  "chance_of_rain": "94",
                  "will_it_snow": 0,
                  "chance_of_snow": "3",
                  "vis_km": 10.2,
                  "vis_miles": 6.0
                },
                {
                  "time_epoch": 1520038800,
                  "time": "2018-03-03 01:00",
                  "temp_c": 1.3,
                  "temp_f": 34.3,
                  "is_day": 0,
                  "condition": {
                    "text": "Fog",
                    "icon": "//cdn.apixu.com/weather/64x64/night/248.png",
                    "code": 1135
                  },
                  "wind_mph": 7.6,
                  "wind_kph": 12.2,
                  "wind_degree": 88,
                  "wind_dir": "E",
                  "pressure_mb": 993.0,
                  "pressure_in": 29.8,
                  "precip_mm": 0.4,
                  "precip_in": 0.02,
                  "humidity": 99,
                  "cloud": 100,
                  "feelslike_c": -2.3,
                  "feelslike_f": 27.9,
                  "windchill_c": -2.3,
                  "windchill_f": 27.9,
                  "heatindex_c": 1.3,
                  "heatindex_f": 34.3,
                  "dewpoint_c": 1.1,
                  "dewpoint_f": 34.0,
                  "will_it_rain": 1,
                  "chance_of_rain": "86",
                  "will_it_snow": 0,
                  "chance_of_snow": "2",
                  "vis_km": 11.1,
                  "vis_miles": 6.0
                },
                {
                  "time_epoch": 1520042400,
                  "time": "2018-03-03 02:00",
                  "temp_c": 1.3,
                  "temp_f": 34.3,
                  "is_day": 0,
                  "condition": {
                    "text": "Light drizzle",
                    "icon": "//cdn.apixu.com/weather/64x64/night/266.png",
                    "code": 1153
                  },
                  "wind_mph": 6.9,
                  "wind_kph": 11.2,
                  "wind_degree": 89,
                  "wind_dir": "E",
                  "pressure_mb": 993.0,
                  "pressure_in": 29.8,
                  "precip_mm": 0.3,
                  "precip_in": 0.01,
                  "humidity": 99,
                  "cloud": 100,
                  "feelslike_c": -2.0,
                  "feelslike_f": 28.4,
                  "windchill_c": -2.0,
                  "windchill_f": 28.4,
                  "heatindex_c": 1.3,
                  "heatindex_f": 34.3,
                  "dewpoint_c": 1.1,
                  "dewpoint_f": 34.0,
                  "will_it_rain": 1,
                  "chance_of_rain": "79",
                  "will_it_snow": 0,
                  "chance_of_snow": "1",
                  "vis_km": 12.1,
                  "vis_miles": 7.0
                },
                {
                  "time_epoch": 1520046000,
                  "time": "2018-03-03 03:00",
                  "temp_c": 1.4,
                  "temp_f": 34.5,
                  "is_day": 0,
                  "condition": {
                    "text": "Fog",
                    "icon": "//cdn.apixu.com/weather/64x64/night/248.png",
                    "code": 1135
                  },
                  "wind_mph": 6.3,
                  "wind_kph": 10.1,
                  "wind_degree": 91,
                  "wind_dir": "E",
                  "pressure_mb": 993.0,
                  "pressure_in": 29.8,
                  "precip_mm": 0.1,
                  "precip_in": 0.0,
                  "humidity": 99,
                  "cloud": 95,
                  "feelslike_c": -1.7,
                  "feelslike_f": 28.9,
                  "windchill_c": -1.7,
                  "windchill_f": 28.9,
                  "heatindex_c": 1.4,
                  "heatindex_f": 34.5,
                  "dewpoint_c": 1.2,
                  "dewpoint_f": 34.2,
                  "will_it_rain": 1,
                  "chance_of_rain": "71",
                  "will_it_snow": 0,
                  "chance_of_snow": "0",
                  "vis_km": 13.0,
                  "vis_miles": 8.0
                }
              ]
            }
          ]
        }
      }
      )json";
      REQUIRE( NLohmannJsonApixu::parseForecast(json, forecast) );

      REQUIRE( forecast.data().size() == 4 );

      REQUIRE( forecast.data()[0].temperatureCelsius() == 1.2f );
      REQUIRE( forecast.data()[0].temperatureFahrenheit() == 34.2f );
      REQUIRE( forecast.data()[0].windSpeed() >= 3.694444f );
      REQUIRE( forecast.data()[0].windSpeed() <= 3.694445f );
      REQUIRE( forecast.data()[0].windDegrees() == 86 );
      REQUIRE( forecast.data()[0].pressure() == 993 );
      REQUIRE( forecast.data()[0].rain() == 0.6f );
      REQUIRE( forecast.data()[0].snow() == 0.0f );
      REQUIRE( forecast.data()[0].humidity() == 98 );
      REQUIRE( forecast.data()[0].cloudiness() == 100 );

      REQUIRE( forecast.data()[1].temperatureCelsius() == 1.3f );
      REQUIRE( forecast.data()[1].temperatureFahrenheit() == 34.3f );
      REQUIRE( forecast.data()[1].windSpeed() >= 3.388888f );
      REQUIRE( forecast.data()[1].windSpeed() <= 3.388889f );
      REQUIRE( forecast.data()[1].windDegrees() == 88 );
      REQUIRE( forecast.data()[1].pressure() == 993 );
      REQUIRE( forecast.data()[1].rain() == 0.4f );
      REQUIRE( forecast.data()[1].snow() == 0.0f );
      REQUIRE( forecast.data()[1].humidity() == 99 );
      REQUIRE( forecast.data()[1].cloudiness() == 100 );

      REQUIRE( forecast.data()[2].temperatureCelsius() == 1.3f );
      REQUIRE( forecast.data()[2].temperatureFahrenheit() == 34.3f );
      REQUIRE( forecast.data()[2].windSpeed() >= 3.111111f );
      REQUIRE( forecast.data()[2].windSpeed() <= 3.111112f );
      REQUIRE( forecast.data()[2].windDegrees() == 89 );
      REQUIRE( forecast.data()[2].pressure() == 993 );
      REQUIRE( forecast.data()[2].rain() == 0.3f );
      REQUIRE( forecast.data()[2].snow() == 0.0f );
      REQUIRE( forecast.data()[2].humidity() == 99 );
      REQUIRE( forecast.data()[2].cloudiness() == 100 );

      REQUIRE( forecast.data()[3].temperatureCelsius() == 1.4f );
      REQUIRE( forecast.data()[3].temperatureFahrenheit() == 34.5f );
      REQUIRE( forecast.data()[3].windSpeed() >= 2.805555f );
      REQUIRE( forecast.data()[3].windSpeed() <= 2.805556f );
      REQUIRE( forecast.data()[3].windDegrees() == 91 );
      REQUIRE( forecast.data()[3].pressure() == 993 );
      REQUIRE( forecast.data()[3].rain() == 0.1f );
      REQUIRE( forecast.data()[3].snow() == 0.0f );
      REQUIRE( forecast.data()[3].humidity() == 99 );
      REQUIRE( forecast.data()[3].cloudiness() == 95 );
    }
  }
}
