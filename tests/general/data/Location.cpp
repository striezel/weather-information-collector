/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2019  Dirk Stolle

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
#include "../../../src/data/Location.hpp"

TEST_CASE("LocationClass")
{
  using namespace wic;
  wic::Location loc;

  SECTION("empty after construction")
  {
    // no data should be set
    REQUIRE_FALSE( loc.hasOwmId() );
    REQUIRE_FALSE( loc.hasCoordinates() );
    REQUIRE_FALSE( loc.hasName() );
    REQUIRE_FALSE( loc.hasCountryCode() );
    REQUIRE_FALSE( loc.hasPostcode() );

    REQUIRE( loc.empty() );
  }

  SECTION("emptiness")
  {
    REQUIRE( loc.empty() );

    SECTION("empty: OpenWeatherMap id")
    {
      REQUIRE( loc.empty() );
      loc.setOwmId(5);
      REQUIRE_FALSE( loc.empty() );
    }

    SECTION("empty: lat. + lon.")
    {
      REQUIRE( loc.empty() );
      loc.setCoordinates(5.0f, 120.0f);
      REQUIRE_FALSE( loc.empty() );
    }

    SECTION("empty: name")
    {
      REQUIRE( loc.empty() );
      loc.setName("Townington");
      REQUIRE_FALSE( loc.empty() );
    }

    SECTION("empty: country code only")
    {
      REQUIRE( loc.empty() );
      loc.setCountryCode("FR");
      // Country code alone does not count.
      REQUIRE( loc.empty() );
    }

    SECTION("empty: name + country code")
    {
      REQUIRE( loc.empty() );
      loc.setName("Townington");
      loc.setCountryCode("UK");
      REQUIRE_FALSE( loc.empty() );
    }

    SECTION("empty: postcode")
    {
      REQUIRE( loc.empty() );
      loc.setPostcode("01234");
      REQUIRE_FALSE( loc.empty() );
    }
  } // emptiness section


  SECTION("set, has + get OpenWeatherMap id")
  {
    loc.setOwmId(12345);
    REQUIRE( loc.hasOwmId() );
    REQUIRE( loc.owmId() == 12345 );
  }

  SECTION("set, has + get latitude and longitude")
  {
    loc.setCoordinates(12.5, 123.4375);
    REQUIRE( loc.hasCoordinates() );
    REQUIRE( loc.latitude() == 12.5 );
    REQUIRE( loc.longitude() == 123.4375 );
  }

  SECTION("out of range latitude")
  {
    loc.setCoordinates(123.0, 123.4);
    REQUIRE_FALSE( loc.hasCoordinates() );

    loc.setCoordinates(-91.2, 123.4);
    REQUIRE_FALSE( loc.hasCoordinates() );
  }

  SECTION("out of range longitude")
  {
    loc.setCoordinates(12.5, 191.2);
    REQUIRE_FALSE( loc.hasCoordinates() );

    loc.setCoordinates(-89.2, -323.2);
    REQUIRE_FALSE( loc.hasCoordinates() );
  }

  SECTION("set, has + get name")
  {
    loc.setName("Townsvillage");
    REQUIRE( loc.hasName() );
    REQUIRE( loc.name() == std::string("Townsvillage") );
  }

  SECTION("set, has + get country code")
  {
    loc.setCountryCode("UK");
    REQUIRE( loc.hasCountryCode() );
    REQUIRE( loc.countryCode() == std::string("UK") );
  }

  SECTION("set, has + get postcode")
  {
    loc.setPostcode("SW1");
    REQUIRE( loc.hasPostcode() );
    REQUIRE( loc.postcode() == std::string("SW1") );
  }

  SECTION("equality for latitude + longitude")
  {
    // both without lat./lon.: equal
    Location loc1, loc2;
    REQUIRE( loc1.equalCoordinates(loc2) );

    // only one with lat./lon.: not equal
    loc1.setCoordinates(9000.1f, 9000.1f);
    loc1.setCoordinates(0.0f, 0.0f);
    REQUIRE_FALSE( loc1.equalCoordinates(loc2) );

    // equal values: equal
    loc1.setCoordinates(5.0, 12.34);
    loc2.setCoordinates(5.0, 12.34);
    REQUIRE( loc1.equalCoordinates(loc2) );

    // almost equal (below 0.01 °): equal
    loc1.setCoordinates(5.009, 12.34);
    loc2.setCoordinates(5.0, 12.34);
    REQUIRE( loc1.equalCoordinates(loc2) );

    // almost equal (below 0.01 °): equal
    loc1.setCoordinates(5.0, 123.409);
    loc2.setCoordinates(5.0, 123.4);
    REQUIRE( loc1.equalCoordinates(loc2) );

    // almost equal (below 0.01 °): equal
    loc1.setCoordinates(5.009, 123.409);
    loc2.setCoordinates(5.0, 123.4);
    REQUIRE( loc1.equalCoordinates(loc2) );

    // difference above 0.01 °: not equal
    loc1.setCoordinates(5.15, 123.4);
    loc2.setCoordinates(5.0, 123.4);
    REQUIRE_FALSE( loc1.equalCoordinates(loc2) );

    //difference above 0.01 °: not equal
    loc1.setCoordinates(5.0, 123.51);
    loc2.setCoordinates(5.0, 123.4);
    REQUIRE_FALSE( loc1.equalCoordinates(loc2) );

    // difference above 0.01 °: not equal
    loc1.setCoordinates(5.12, 123.51);
    loc2.setCoordinates(5.0, 123.4);
    REQUIRE_FALSE( loc1.equalCoordinates(loc2) );
  }

  SECTION("equality operator")
  {
    Location loc1, loc2;
    REQUIRE( loc1 == loc2 );

    loc1.setCoordinates(12.0, 123.4);
    REQUIRE_FALSE( loc1 == loc2 );
    loc2.setCoordinates(12.0, 123.4);
    REQUIRE( loc1 == loc2 );

    loc1.setPostcode("SW1");
    REQUIRE_FALSE( loc1 == loc2 );
    loc2.setPostcode("SW1");
    REQUIRE( loc1 == loc2 );

    loc1.setName("Town");
    REQUIRE_FALSE( loc1 == loc2 );
    loc2.setName("Town");
    REQUIRE( loc1 == loc2 );

    loc1.setCountryCode("TO");
    REQUIRE_FALSE( loc1 == loc2 );
    loc2.setCountryCode("TO");
    REQUIRE( loc1 == loc2 );

    loc1.setOwmId(5);
    REQUIRE_FALSE( loc1 == loc2 );
    loc2.setOwmId(4);
    REQUIRE_FALSE( loc1 == loc2 );
    loc2.setOwmId(5);
    REQUIRE( loc1 == loc2 );
  }
}
