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
#include "../../../src/util/encode.hpp"

TEST_CASE("URL-encoding")
{
  using namespace wic;

  SECTION("urlEncode")
  {
    SECTION("empty and whitespace strings")
    {
      REQUIRE( urlEncode("") == "" );
      REQUIRE( urlEncode(" ") == "%20" );
      REQUIRE( urlEncode("  \t\r\n") == "%20%20%09%0D%0A" );
    }

    SECTION("strings that do not need encoding")
    {
      REQUIRE( urlEncode("foo") == "foo" );
      REQUIRE( urlEncode("Bar") == "Bar" );
      REQUIRE( urlEncode("abcdefghijklmnopqrstuvwxyz") == "abcdefghijklmnopqrstuvwxyz" );
      REQUIRE( urlEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
      REQUIRE( urlEncode("0123456789") == "0123456789" );
      REQUIRE( urlEncode("~foo_bar-baz.quux") == "~foo_bar-baz.quux" );
    }

    SECTION("percent sign")
    {
      REQUIRE( urlEncode("%") == "%25" );
    }

    SECTION("strings that need encoding")
    {
      REQUIRE( urlEncode("New York") == "New%20York" );
      REQUIRE( urlEncode("Tōkyō") == "T%C5%8Dky%C5%8D" );
      REQUIRE( urlEncode("Üärgß!") == "%C3%9C%C3%A4rg%C3%9F%21" );

    }
  }
}
