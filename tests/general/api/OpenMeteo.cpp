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
#include "../../../src/api/OpenMeteo.hpp"

TEST_CASE("OpenMeteo - non-network tests")
{
  using namespace wic;

  OpenMeteo api;

  SECTION("validLocation")
  {
    Location location;

    SECTION("empty location")
    {
      REQUIRE_FALSE( api.validLocation(location) );
    }

    SECTION("OpenWeatherMap id")
    {
      location.setOwmId(123);
      REQUIRE_FALSE( api.validLocation(location) );
    }

    SECTION("country code")
    {
      location.setCountryCode("FR");
      REQUIRE_FALSE( api.validLocation(location) );
    }

    SECTION("latitude + longitude")
    {
      location.setCoordinates(12.34f, 56.78f);
      REQUIRE( api.validLocation(location) );
    }

    SECTION("name is set")
    {
      location.setName("Some City");
      REQUIRE_FALSE( api.validLocation(location) );
    }

    SECTION("postcode is set")
    {
      location.setPostcode("ABC123");
      REQUIRE_FALSE( api.validLocation(location) );
    }
  }

  SECTION("setApiKey")
  {
    // This is just a bogus test. Open-Meteo uses no key.
    api.setApiKey("foo1bar2baz3");
  }

  SECTION("toRequestString")
  {
    Location location;

    SECTION("no data set")
    {
      REQUIRE( OpenMeteo::toRequestString(location).empty() );
    }

    SECTION("insufficient data set")
    {
      location.setCountryCode("DK");
      REQUIRE( OpenMeteo::toRequestString(location).empty() );
    }

    SECTION("latitude + longitude")
    {
      location.setCoordinates(12.34f, 56.78f);
      REQUIRE( OpenMeteo::toRequestString(location) == "latitude=12.34&longitude=56.78" );
    }
  }
}
