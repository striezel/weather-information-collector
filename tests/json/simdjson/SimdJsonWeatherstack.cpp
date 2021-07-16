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
  }

  SECTION("parseForecast")
  {
    // Note: Forecast data retrieval is not implemented for Weatherstack.
  }
}
