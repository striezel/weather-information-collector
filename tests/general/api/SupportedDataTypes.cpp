/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2021, 2022, 2025  Dirk Stolle

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
#include "../../../src/api/Apixu.hpp"
#include "../../../src/api/DarkSky.hpp"
#include "../../../src/api/OpenMeteo.hpp"
#include "../../../src/api/OpenWeatherMap.hpp"
#include "../../../src/api/Weatherbit.hpp"
#include "../../../src/api/Weatherstack.hpp"

TEST_CASE("Check supported data types of API implementations")
{
  using namespace wic;

  SECTION("Apixu")
  {
    Apixu api("This is not a valid key.");
    // Apixu supports all valid types.
    REQUIRE( api.supportsDataType(DataType::Current) );
    REQUIRE( api.supportsDataType(DataType::Forecast) );
    REQUIRE( api.supportsDataType(DataType::CurrentAndForecast) );
    // None is never supported.
    REQUIRE_FALSE( api.supportsDataType(DataType::none) );
  }

  SECTION("DarkSky")
  {
    DarkSky ds("This is not a valid key.");
    // DarkSky supports all valid types.
    REQUIRE( ds.supportsDataType(DataType::Current) );
    REQUIRE( ds.supportsDataType(DataType::Forecast) );
    REQUIRE( ds.supportsDataType(DataType::CurrentAndForecast) );
    // None is never supported.
    REQUIRE_FALSE( ds.supportsDataType(DataType::none) );
  }

  SECTION("Open-Meteo")
  {
    OpenMeteo om;
    // Open-Meteo supports all valid types.
    REQUIRE( om.supportsDataType(DataType::Current) );
    REQUIRE( om.supportsDataType(DataType::Forecast) );
    REQUIRE( om.supportsDataType(DataType::CurrentAndForecast) );
    // None is never supported.
    REQUIRE_FALSE( om.supportsDataType(DataType::none) );
  }

  SECTION("OpenWeatherMap")
  {
    OpenWeatherMap owm("This is not a valid key.");
    // OpenWeatherMap supports current and forecast data.
    REQUIRE( owm.supportsDataType(DataType::Current) );
    REQUIRE( owm.supportsDataType(DataType::Forecast) );
    // OpenWeatherMap's "one-call" API is currently not implemented, so this
    // is not supported at the moment.
    REQUIRE_FALSE( owm.supportsDataType(DataType::CurrentAndForecast) );
    // None is never supported.
    REQUIRE_FALSE( owm.supportsDataType(DataType::none) );
  }

  SECTION("Weatherbit")
  {
    Weatherbit wb(PlanWeatherbit::Standard, "This is not a valid key.");
    // Weatherbit supports current and forecast data.
    REQUIRE( wb.supportsDataType(DataType::Current) );
    REQUIRE( wb.supportsDataType(DataType::Forecast) );
    // Mixed data retrieval is not available for Weatherbit implementation.
    REQUIRE_FALSE( wb.supportsDataType(DataType::CurrentAndForecast) );
    // None is never supported.
    REQUIRE_FALSE( wb.supportsDataType(DataType::none) );
  }

  SECTION("Weatherstack")
  {
    Weatherstack ws(PlanWeatherstack::Standard, "This is not a valid key.");
    // Weatherstack supports current data only.
    REQUIRE( ws.supportsDataType(DataType::Current) );
    REQUIRE_FALSE( ws.supportsDataType(DataType::Forecast) );
    REQUIRE_FALSE( ws.supportsDataType(DataType::CurrentAndForecast) );
    // None is never supported.
    REQUIRE_FALSE( ws.supportsDataType(DataType::none) );
  }
}
