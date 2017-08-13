/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017  Dirk Stolle

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
#include "../../src/data/Location.hpp"

TEST_CASE("LocationClass")
{
  using namespace wic;
  wic::Location loc;

  SECTION("empty after construction")
  {
    //no data should be set
    REQUIRE_FALSE( loc.hasId() );
    REQUIRE_FALSE( loc.hasLatitudeAndLongitude() );
    REQUIRE_FALSE( loc.hasName() );
    REQUIRE_FALSE( loc.hasPostcode() );
  }

  SECTION("set, has + get id")
  {
    loc.setId(12345);
    REQUIRE( loc.hasId() );
    REQUIRE( loc.id() == 12345 );
  }

  SECTION("set, has + get latitude and longitude")
  {
    loc.setLatitudeLongitude(12.5, 123.4375);
    REQUIRE( loc.hasLatitudeAndLongitude() );
    REQUIRE( loc.latitude() == 12.5 );
    REQUIRE( loc.longitude() == 123.4375 );
  }

  SECTION("out of range latitude")
  {
    loc.setLatitudeLongitude(123.0, 123.4);
    REQUIRE_FALSE( loc.hasLatitudeAndLongitude() );

    loc.setLatitudeLongitude(-91.2, 123.4);
    REQUIRE_FALSE( loc.hasLatitudeAndLongitude() );
  }

  SECTION("out of range longitude")
  {
    loc.setLatitudeLongitude(12.5, 191.2);
    REQUIRE_FALSE( loc.hasLatitudeAndLongitude() );

    loc.setLatitudeLongitude(-89.2, -323.2);
    REQUIRE_FALSE( loc.hasLatitudeAndLongitude() );
  }

  SECTION("set, has + get name")
  {
    loc.setName("Townsvillage");
    REQUIRE( loc.hasName() );
    REQUIRE( loc.name() == std::string("Townsvillage") );
  }

  SECTION("set, has + get postcode")
  {
    loc.setPostcode("SW1");
    REQUIRE( loc.hasPostcode() );
    REQUIRE( loc.postcode() == std::string("SW1") );
  }
}
