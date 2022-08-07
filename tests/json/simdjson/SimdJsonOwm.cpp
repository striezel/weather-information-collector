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
#include "../../../src/json/SimdJsonOwm.hpp"

TEST_CASE("Class SimdJsonOwm")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("", weather) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("{ }", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather("\t\t\t\t", weather) );
    }

    SECTION("missing main object")
    {
      const std::string json = R"json(
      {
        "coord": {
          "lon":13.74,
          "lat":51.05
        },
        "weather": [
          {
            "id":502,
            "main":"Rain",
            "description":"heavy intensity rain",
            "icon":"10d"
          }
        ],
        "base": "stations",
        "visibility":8000,
        "wind": {
          "speed":3.1,
          "deg":220
        },
        "rain": {
          "3h":17.08
        },
        "clouds": {
          "all":92
        },
        "dt":1503730200,
        "sys": {
          "type":1,
          "id":4879,
          "message":0.0219,
          "country":"DE",
          "sunrise":1503720574,
          "sunset":1503770568
        },
        "id":2935022,
        "name":"Dresden",
        "cod":200
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather(json, weather) );
    }

    SECTION("main is not an object")
    {
      const std::string json = R"json(
      {
        "coord": {
          "lon":13.74,
          "lat":51.05
        },
        "weather": [
          {
            "id":502,
            "main":"Rain",
            "description":"heavy intensity rain",
            "icon":"10d"
          }
        ],
        "base": "stations",
        "main": "over 9000",
        "visibility":8000,
        "wind": {
          "speed":3.1,
          "deg":220
        },
        "rain": {
          "3h":17.08
        },
        "clouds": {
          "all":92
        },
        "dt":1503730200,
        "sys": {
          "type":1,
          "id":4879,
          "message":0.0219,
          "country":"DE",
          "sunrise":1503720574,
          "sunset":1503770568
        },
        "id":2935022,
        "name":"Dresden",
        "cod":200
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseCurrentWeather(json, weather) );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("", forecast) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("{ }", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("    ", forecast) );
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("\n", forecast) );
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("\r", forecast) );
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( SimdJsonOwm::parseForecast("\t\t\t\t", forecast) );
    }

    SECTION("missing main object in one element")
    {
      const std::string json = R"json(
      {
          "cod": "200",
          "message": 0.0023,
          "cnt": 4,
          "list": [
              {
                  "dt": 1532984400,
                  "main":{"temp":300.38,"temp_min":298.334,"temp_max":300.38,"pressure":993.38,"sea_level":1030.57,"grnd_level":993.38,"humidity":58,"temp_kf":2.05},
                  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],
                  "clouds":{"all":56},
                  "wind":{"speed":2.06,"deg":92.0038},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-30 21:00:00"
              },
              {
                  "dt": 1532995200,
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.59,"deg":108.505},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 00:00:00"
              },
              {
                  "dt":1533006000,
                  "main":{"temp":293.64,"temp_min":292.955,"temp_max":293.64,"pressure":993.81,"sea_level":1031.24,"grnd_level":993.81,"humidity":72,"temp_kf":0.68},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.26,"deg":125.501},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 03:00:00"
              },
              {
                  "dt":1533016800,
                  "main":{"temp":298.226,"temp_min":298.226,"temp_max":298.226,"pressure":993.98,"sea_level":1031.32,"grnd_level":993.98,"humidity":50,"temp_kf":0},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01d"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.16,"deg":148.501},
                  "rain":{},
                  "sys":{"pod":"d"},
                  "dt_txt":"2018-07-31 06:00:00"
              }
          ],
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }

    SECTION("main is not an object in one element")
    {
      const std::string json = R"json(
      {
          "cod": "200",
          "message": 0.0023,
          "cnt": 3,
          "list": [
              {
                  "dt": 1532984400,
                  "main":{"temp":300.38,"temp_min":298.334,"temp_max":300.38,"pressure":993.38,"sea_level":1030.57,"grnd_level":993.38,"humidity":58,"temp_kf":2.05},
                  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],
                  "clouds":{"all":56},
                  "wind":{"speed":2.06,"deg":92.0038},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-30 21:00:00"
              },
              {
                  "dt": 1532995200,
                  "main": 1234,
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.59,"deg":108.505},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 00:00:00"
              },
              {
                  "dt":1533006000,
                  "main":{"temp":293.64,"temp_min":292.955,"temp_max":293.64,"pressure":993.81,"sea_level":1031.24,"grnd_level":993.81,"humidity":72,"temp_kf":0.68},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.26,"deg":125.501},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 03:00:00"
              }
          ],
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }

    SECTION("list is missing")
    {
      const std::string json = R"json(
      {
          "cod": "200",
          "message": 0.0023,
          "cnt": 3,
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }

    SECTION("list is not an array")
    {
      const std::string json = R"json(
      {
          "cod": "200",
          "message": 0.0023,
          "cnt": 3,
          "list": { "one": 1, "two": 2, "three": 3 },
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );

      const std::string json2 = R"json(
      {
          "cod": "200",
          "message": 0.0023,
          "cnt": 3,
          "list": true,
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json2, forecast) );
    }

    SECTION("cnt is missing")
    {
      const std::string json = R"json(
            {
          "cod": "200",
          "message": 0.0023,
          "list": [
              {
                  "dt": 1532984400,
                  "main":{"temp":300.38,"temp_min":298.334,"temp_max":300.38,"pressure":993.38,"sea_level":1030.57,"grnd_level":993.38,"humidity":58,"temp_kf":2.05},
                  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],
                  "clouds":{"all":56},
                  "wind":{"speed":2.06,"deg":92.0038},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-30 21:00:00"
              },
              {
                  "dt": 1532995200,
                  "main":{"temp":296.23,"temp_min":294.865,"temp_max":296.23,"pressure":993.71,"sea_level":1031.08,"grnd_level":993.71,"humidity":63,"temp_kf":1.36},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.59,"deg":108.505},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 00:00:00"
              },
              {
                  "dt":1533006000,
                  "main":{"temp":293.64,"temp_min":292.955,"temp_max":293.64,"pressure":993.81,"sea_level":1031.24,"grnd_level":993.81,"humidity":72,"temp_kf":0.68},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.26,"deg":125.501},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 03:00:00"
              }
          ],
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }

    SECTION("cnt is not an integer")
    {
      const std::string json = R"json(
            {
          "cod": "200",
          "message": 0.0023,
          "cnt": 1.0123,
          "list": [
              {
                  "dt": 1532984400,
                  "main":{"temp":300.38,"temp_min":298.334,"temp_max":300.38,"pressure":993.38,"sea_level":1030.57,"grnd_level":993.38,"humidity":58,"temp_kf":2.05},
                  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],
                  "clouds":{"all":56},
                  "wind":{"speed":2.06,"deg":92.0038},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-30 21:00:00"
              },
              {
                  "dt": 1532995200,
                  "main":{"temp":296.23,"temp_min":294.865,"temp_max":296.23,"pressure":993.71,"sea_level":1031.08,"grnd_level":993.71,"humidity":63,"temp_kf":1.36},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.59,"deg":108.505},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 00:00:00"
              },
              {
                  "dt":1533006000,
                  "main":{"temp":293.64,"temp_min":292.955,"temp_max":293.64,"pressure":993.81,"sea_level":1031.24,"grnd_level":993.81,"humidity":72,"temp_kf":0.68},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.26,"deg":125.501},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 03:00:00"
              }
          ],
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }

    SECTION("cnt does not match the number of actual elements")
    {
      const std::string json = R"json(
            {
          "cod": "200",
          "message": 0.0023,
          "cnt": 12,
          "list": [
              {
                  "dt": 1532984400,
                  "main":{"temp":300.38,"temp_min":298.334,"temp_max":300.38,"pressure":993.38,"sea_level":1030.57,"grnd_level":993.38,"humidity":58,"temp_kf":2.05},
                  "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],
                  "clouds":{"all":56},
                  "wind":{"speed":2.06,"deg":92.0038},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-30 21:00:00"
              },
              {
                  "dt": 1532995200,
                  "main":{"temp":296.23,"temp_min":294.865,"temp_max":296.23,"pressure":993.71,"sea_level":1031.08,"grnd_level":993.71,"humidity":63,"temp_kf":1.36},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.59,"deg":108.505},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 00:00:00"
              },
              {
                  "dt":1533006000,
                  "main":{"temp":293.64,"temp_min":292.955,"temp_max":293.64,"pressure":993.81,"sea_level":1031.24,"grnd_level":993.81,"humidity":72,"temp_kf":0.68},
                  "weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],
                  "clouds":{"all":0},
                  "wind":{"speed":2.26,"deg":125.501},
                  "rain":{},
                  "sys":{"pod":"n"},
                  "dt_txt":"2018-07-31 03:00:00"
              }
          ],
          "city":
          {
              "id": 2935022,
              "name": "Dresden",
              "coord":
              {
                  "lat": 51.0493,
                  "lon": 13.7381
              },
              "country": "DE",
              "population": 486854
          }
      }
      )json";
      REQUIRE_FALSE( SimdJsonOwm::parseForecast(json, forecast) );
    }
  }

  SECTION("parseLocations")
  {
    std::vector<std::pair<Location, Weather> > locations;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("", locations) );
    }

    SECTION("empty JSON object")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("{ }", locations) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("    ", locations) );
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("\n", locations) );
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("\r", locations) );
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("\r\n\r\n", locations) );
      REQUIRE_FALSE( SimdJsonOwm::parseLocations("\t\t\t\t", locations) );
    }

    SECTION("count is missing")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "list": [
          {
            "id": 2878037,
            "name": "Lichtenstein",
            "coord": {
              "lat": 50.75,
              "lon": 12.65
            },
            "main": {
              "temp": 288.87,
              "feels_like": 287.78,
              "temp_min": 286.79,
              "temp_max": 290.71,
              "pressure": 1022,
              "humidity": 49,
              "sea_level": 1022,
              "grnd_level": 975
            },
            "dt": 1659902950,
            "wind": {
              "speed": 2.8,
              "deg": 97
            },
            "sys": {
              "country": "DE"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          },
          {
            "id": 3042058,
            "name": "Principality of Liechtenstein",
            "coord": {
              "lat": 47.1667,
              "lon": 9.5333
            },
            "main": {
              "temp": 291.25,
              "feels_like": 291.18,
              "temp_min": 289.2,
              "temp_max": 292.16,
              "pressure": 1021,
              "humidity": 79
            },
            "dt": 1659902816,
            "wind": {
              "speed": 0.45,
              "deg": 232
            },
            "sys": {
              "country": "LI"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          }
        ]
      }
      )json";

      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("count is not an integer")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": "foo",
        "list": [
          {
            "id": 2878037,
            "name": "Lichtenstein",
            "coord": {
              "lat": 50.75,
              "lon": 12.65
            },
            "main": {
              "temp": 288.87,
              "feels_like": 287.78,
              "temp_min": 286.79,
              "temp_max": 290.71,
              "pressure": 1022,
              "humidity": 49,
              "sea_level": 1022,
              "grnd_level": 975
            },
            "dt": 1659902950,
            "wind": {
              "speed": 2.8,
              "deg": 97
            },
            "sys": {
              "country": "DE"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          },
          {
            "id": 3042058,
            "name": "Principality of Liechtenstein",
            "coord": {
              "lat": 47.1667,
              "lon": 9.5333
            },
            "main": {
              "temp": 291.25,
              "feels_like": 291.18,
              "temp_min": 289.2,
              "temp_max": 292.16,
              "pressure": 1021,
              "humidity": 79
            },
            "dt": 1659902816,
            "wind": {
              "speed": 0.45,
              "deg": 232
            },
            "sys": {
              "country": "LI"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          }
        ]
      }
      )json";

      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("list element is missing")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": 2
      }
      )json";

      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("list element is not an array")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": 2,
        "list": 123
      }
      )json";

      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("location data is empty")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": 1,
        "list": [
          {
            "main": {
              "temp": 288.87,
              "feels_like": 287.78,
              "temp_min": 286.79,
              "temp_max": 290.71,
              "pressure": 1022,
              "humidity": 49,
              "sea_level": 1022,
              "grnd_level": 975
            },
            "dt": 1659902950,
            "wind": {
              "speed": 2.8,
              "deg": 97
            },
            "sys": {
              "country": "DE"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          }
        ]
      }
      )json";

      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("weather data is missing for one element")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": 2,
        "list": [
          {
            "id": 2878037,
            "name": "Lichtenstein",
            "coord": {
              "lat": 50.75,
              "lon": 12.65
            },
            "sys": {
              "country": "DE"
            }
          },
          {
            "id": 3042058,
            "name": "Principality of Liechtenstein",
            "coord": {
              "lat": 47.1667,
              "lon": 9.5333
            },
            "main": {
              "temp": 291.25,
              "feels_like": 291.18,
              "temp_min": 289.2,
              "temp_max": 292.16,
              "pressure": 1021,
              "humidity": 79
            },
            "dt": 1659902816,
            "wind": {
              "speed": 0.45,
              "deg": 232
            },
            "sys": {
              "country": "LI"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          }
        ]
      }
      )json";

      REQUIRE_FALSE( SimdJsonOwm::parseLocations(json, locations) );
    }

    SECTION("successful parsing with zero elements")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": 0,
        "list": []
      }
      )json";

      REQUIRE( SimdJsonOwm::parseLocations(json, locations) );
      REQUIRE( locations.size() == 0 );
    }

    SECTION("successful parsing with some matching elements")
    {
      const std::string json = R"json(
      {
        "message": "accurate",
        "cod": "200",
        "count": 2,
        "list": [
          {
            "id": 2878037,
            "name": "Lichtenstein",
            "coord": {
              "lat": 50.75,
              "lon": 12.65
            },
            "main": {
              "temp": 288.87,
              "feels_like": 287.78,
              "temp_min": 286.79,
              "temp_max": 290.71,
              "pressure": 1022,
              "humidity": 49,
              "sea_level": 1022,
              "grnd_level": 975
            },
            "dt": 1659902950,
            "wind": {
              "speed": 2.8,
              "deg": 97
            },
            "sys": {
              "country": "DE"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          },
          {
            "id": 3042058,
            "name": "Principality of Liechtenstein",
            "coord": {
              "lat": 47.1667,
              "lon": 9.5333
            },
            "main": {
              "temp": 291.25,
              "feels_like": 291.18,
              "temp_min": 289.2,
              "temp_max": 292.16,
              "pressure": 1021,
              "humidity": 79
            },
            "dt": 1659902816,
            "wind": {
              "speed": 0.45,
              "deg": 232
            },
            "sys": {
              "country": "LI"
            },
            "rain": null,
            "snow": null,
            "clouds": {
              "all": 100
            },
            "weather": [
              {
                "id": 804,
                "main": "Clouds",
                "description": "overcast clouds",
                "icon": "04n"
              }
            ]
          }
        ]
      }
      )json";

      REQUIRE( SimdJsonOwm::parseLocations(json, locations) );
      REQUIRE( locations.size() == 2 );

      REQUIRE( locations[0].first.owmId() == 2878037 );
      REQUIRE( locations[0].first.latitude() == 50.75f );
      REQUIRE( locations[0].first.longitude() == 12.65f );
      REQUIRE( locations[0].first.name() == "Lichtenstein" );
      REQUIRE( locations[0].first.countryCode() == "DE" );
      REQUIRE( locations[0].first.postcode().empty() );

      REQUIRE( locations[0].second.temperatureKelvin() == 288.87f );
      REQUIRE( locations[0].second.temperatureCelsius() == 15.72f );
      REQUIRE( locations[0].second.temperatureFahrenheit() == 60.296f );
      REQUIRE( locations[0].second.humidity() == 49 );
      REQUIRE_FALSE( locations[0].second.hasRain() );
      REQUIRE_FALSE( locations[0].second.hasSnow() );
      REQUIRE( locations[0].second.pressure() == 1022 );
      REQUIRE( locations[0].second.windSpeed() == 2.8f );
      REQUIRE( locations[0].second.windDegrees() == 97 );
      REQUIRE( locations[0].second.cloudiness() == 100 );

      REQUIRE( locations[1].first.owmId() == 3042058 );
      REQUIRE( locations[1].first.latitude() == 47.1667f );
      REQUIRE( locations[1].first.longitude() == 9.5333f );
      REQUIRE( locations[1].first.name() == "Principality of Liechtenstein" );
      REQUIRE( locations[1].first.countryCode() == "LI" );
      REQUIRE( locations[1].first.postcode().empty() );

      REQUIRE( locations[1].second.temperatureKelvin() == 291.25f );
      REQUIRE( locations[1].second.temperatureCelsius() == 18.1f );
      REQUIRE( locations[1].second.temperatureFahrenheit() == 64.58f );
      REQUIRE( locations[1].second.humidity() == 79 );
      REQUIRE_FALSE( locations[1].second.hasRain() );
      REQUIRE_FALSE( locations[1].second.hasSnow() );
      REQUIRE( locations[1].second.pressure() == 1021 );
      REQUIRE( locations[1].second.windSpeed() == 0.45f );
      REQUIRE( locations[1].second.windDegrees() == 232 );
      REQUIRE( locations[1].second.cloudiness() == 100 );
    }
  }
}
