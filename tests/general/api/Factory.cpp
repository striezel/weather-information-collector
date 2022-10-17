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
#include "../../../src/api/Factory.hpp"

TEST_CASE("Factory gonna factor, right? ;P")
{
  using namespace wic;

  // All API types except "none" should return a non-null unique pointer.

  SECTION("Apixu")
  {
    const auto apixu = Factory::create(ApiType::Apixu, PlanWeatherbit::none, PlanWeatherstack::none, "No key.");
    REQUIRE( apixu );
  }

  SECTION("DarkSky")
  {
    const auto dark = Factory::create(ApiType::DarkSky, PlanWeatherbit::none, PlanWeatherstack::none, "No key.");
    REQUIRE( dark );
  }

  SECTION("Open-Meteo")
  {
    const auto open = Factory::create(ApiType::OpenMeteo, PlanWeatherbit::none, PlanWeatherstack::Standard, "No key.");
    REQUIRE( open );
  }

  SECTION("OpenWeatherMap")
  {
    const auto open = Factory::create(ApiType::OpenWeatherMap, PlanWeatherbit::none, PlanWeatherstack::none, "No key.");
    REQUIRE( open );
  }

  SECTION("Weatherbit")
  {
    const auto wbit = Factory::create(ApiType::Weatherbit, PlanWeatherbit::Starter, PlanWeatherstack::none, "No key.");
    REQUIRE( wbit );
  }

  SECTION("Weatherstack")
  {
    const auto wstack = Factory::create(ApiType::Weatherstack, PlanWeatherbit::none, PlanWeatherstack::Standard, "No key.");
    REQUIRE( wstack );
  }

  SECTION("None")
  {
    const auto theNone = Factory::create(ApiType::none, PlanWeatherbit::none, PlanWeatherstack::none, "No key.");
    REQUIRE_FALSE( theNone );
  }
}
