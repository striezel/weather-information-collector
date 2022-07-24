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
#include <limits>
#include "../../../src/json/ApixuFunctions.hpp"

TEST_CASE("ApixuFunctions")
{
  using namespace wic;

  SECTION("precipitationDistinction")
  {
    Weather weather;

    SECTION("temperature above zero")
    {
      weather.setTemperatureCelsius(25.0f);
      precipitationDistinction(5.0f, weather);
      REQUIRE( weather.rain() == 5.0f );
      REQUIRE( weather.snow() == 0.0f );
    }

    SECTION("temperature is zero")
    {
      weather.setTemperatureCelsius(0.0f);
      precipitationDistinction(15.0f, weather);
      REQUIRE( weather.rain() == 15.0f );
      REQUIRE( weather.snow() == 0.0f );
    }

    SECTION("temperature below zero")
    {
      weather.setTemperatureCelsius(-2.5f);
      precipitationDistinction(5.0f, weather);
      REQUIRE( weather.rain() == 0.0f );
      REQUIRE( weather.snow() == 5.0f );
    }

    SECTION("temperature is not set")
    {
      weather.setTemperatureCelsius(std::numeric_limits<float>::quiet_NaN());
      precipitationDistinction(3.0f, weather);
      REQUIRE( weather.rain() == 3.0f );
      REQUIRE( weather.snow() == 0.0f );
    }
  }
}
