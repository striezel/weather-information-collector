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
#include "../../../src/api/Weatherstack.hpp"

TEST_CASE("Weatherstack - non-network tests")
{
  using namespace wic;

  Weatherstack api(PlanWeatherstack::Free, "not a valid key");

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
      location.setCountryCode("DK");
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
      REQUIRE( api.validLocation(location) );
    }

    SECTION("name + country code are set")
    {
      location.setName("Some City");
      location.setCountryCode("DK");
      REQUIRE( api.validLocation(location) );
    }

    SECTION("postcode is set")
    {
      location.setPostcode("ABC123");
      REQUIRE( api.validLocation(location) );
    }

    SECTION("postcode + country code are set")
    {
      location.setPostcode("ABC123");
      location.setCountryCode("NL");
      REQUIRE( api.validLocation(location) );
    }
  }

  SECTION("setApiKey")
  {
    // This is just a bogus test. Key is encapsulated, without getter, so there
    // is no real way to check it.
    api.setApiKey("foo1bar2baz3");
  }

  SECTION("toRequestString")
  {
    Location location;

    SECTION("no data set")
    {
      REQUIRE( Weatherstack::toRequestString(location).empty() );
    }

    SECTION("insufficient data set")
    {
      location.setCountryCode("DK");
      REQUIRE( Weatherstack::toRequestString(location).empty() );
    }

    SECTION("latitude + longitude")
    {
      location.setCoordinates(12.34f, 56.78f);
      REQUIRE( Weatherstack::toRequestString(location) == "query=12.34,56.78" );
    }

    SECTION("name + country are set")
    {
      location.setName("Mexico City");
      location.setCountryCode("MX");
      REQUIRE( Weatherstack::toRequestString(location) == "query=Mexico City" );
    }

    SECTION("postcode + country are set")
    {
      location.setPostcode("ABC123");
      location.setCountryCode("DK");
      REQUIRE( Weatherstack::toRequestString(location) == "query=ABC123" );
    }
  }

  SECTION("currentWeather")
  {
    Location location;
    location.setCoordinates(12.34f, 56.78f);

    SECTION("request with empty API key fails")
    {
      api.setApiKey("");
      Weather w;
      REQUIRE_FALSE( api.currentWeather(location, w) );
    }
  }

  SECTION("forecastWeather")
  {
    Location location;
    location.setCoordinates(12.34f, 56.78f);

    SECTION("request with empty API key fails")
    {
      api.setApiKey("");
      Forecast fc;
      REQUIRE_FALSE( api.forecastWeather(location, fc) );
    }
  }

  SECTION("currentAndForecastWeather")
  {
    Location location;
    location.setCoordinates(12.34f, 56.78f);

    SECTION("request with empty API key fails")
    {
      api.setApiKey("");
      Weather w;
      Forecast fc;
      REQUIRE_FALSE( api.currentAndForecastWeather(location, w, fc) );
    }
  }
}