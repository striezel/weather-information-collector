/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2021  Dirk Stolle

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

#include <catch.hpp>
#include "../../../src/json/SimdJsonApixu.hpp"

TEST_CASE("Class SimdJsonApixu")
{
  using namespace wic;

  SECTION("parseCurrentWeather")
  {
    Weather weather;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather(json, weather) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather("", weather) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather("    ", weather) );
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather("\n", weather) );
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather("\r", weather) );
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather("\r\n\r\n", weather) );
      REQUIRE_FALSE( SimdJsonApixu::parseCurrentWeather("\t\t\t\t", weather) );
    }
  }

  SECTION("parseForecast")
  {
    Forecast forecast;

    SECTION("not valid JSON")
    {
      const std::string json = "{ \"this\": 'is not valid, JSON: true";
      REQUIRE_FALSE( SimdJsonApixu::parseForecast(json, forecast) );
    }

    SECTION("empty string")
    {
      REQUIRE_FALSE( SimdJsonApixu::parseForecast("", forecast) );
    }

    SECTION("whitespace strings")
    {
      REQUIRE_FALSE( SimdJsonApixu::parseForecast("    ", forecast) );
      REQUIRE_FALSE( SimdJsonApixu::parseForecast("\n", forecast) );
      REQUIRE_FALSE( SimdJsonApixu::parseForecast("\r", forecast) );
      REQUIRE_FALSE( SimdJsonApixu::parseForecast("\r\n\r\n", forecast) );
      REQUIRE_FALSE( SimdJsonApixu::parseForecast("\t\t\t\t", forecast) );
    }
  }
}
