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
#include "../../../src/json/OpenMeteoFunctions.hpp"

TEST_CASE("OpenMeteoFunctions")
{
  using namespace wic;

  SECTION("isoStringToTime")
  {
    SECTION("invalid format: missing dashes or missing colon")
    {
      auto opt = isoStringToTime("2022F10-31T12:34");
      REQUIRE_FALSE( opt.has_value() );

      opt = isoStringToTime("2022-10F31T12:34");
      REQUIRE_FALSE( opt.has_value() );

      opt = isoStringToTime("2022-10-31T12F34");
      REQUIRE_FALSE( opt.has_value() );
    }

    SECTION("invalid data: parts are not numbers")
    {
      auto opt = isoStringToTime("202F-10-31T12:34");
      REQUIRE_FALSE( opt.has_value() );

      opt = isoStringToTime("2022-1F-31T12:34");
      REQUIRE_FALSE( opt.has_value() );

      opt = isoStringToTime("2022-10-1FT12:34");
      REQUIRE_FALSE( opt.has_value() );

      opt = isoStringToTime("2022-10-31T1F:34");
      REQUIRE_FALSE( opt.has_value() );

      opt = isoStringToTime("2022-10-31T12:3C");
      REQUIRE_FALSE( opt.has_value() );
    }

    SECTION("invalid data: parts are not numbers")
    {
      const auto opt = isoStringToTime("FAIL-10-31T12:34");
      REQUIRE_FALSE( opt.has_value() );
    }

    SECTION("valid data")
    {
      auto opt = isoStringToTime("2022-10-18T23:45");
      REQUIRE( opt.has_value() );
      auto tt = std::chrono::system_clock::to_time_t(opt.value());
      REQUIRE( tt != static_cast<std::time_t>(-1) );
      std::tm tm{};
      auto tm_ptr = std::localtime(&tt);
      REQUIRE( tm_ptr != nullptr );
      tm = *tm_ptr;
      REQUIRE( tm.tm_year == 2022 - 1900 );
      REQUIRE( tm.tm_mon == 10 - 1 );
      REQUIRE( tm.tm_mday == 18 );
      REQUIRE( tm.tm_hour == 23 );
      REQUIRE( tm.tm_min == 45 );

      opt = isoStringToTime("2038-12-31T01:23");
      REQUIRE( opt.has_value() );
      tt = std::chrono::system_clock::to_time_t(opt.value());
      REQUIRE( tt != static_cast<std::time_t>(-1) );
      tm_ptr = std::localtime(&tt);
      REQUIRE( tm_ptr != nullptr );
      tm = *tm_ptr;
      REQUIRE( tm.tm_year == 2038 - 1900 );
      REQUIRE( tm.tm_mon == 12 - 1 );
      REQUIRE( tm.tm_mday == 31 );
      REQUIRE( tm.tm_hour == 1 );
      REQUIRE( tm.tm_min == 23 );
    }
  }
}
