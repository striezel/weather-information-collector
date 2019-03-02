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
#include "../../../src/util/Strings.hpp"

TEST_CASE("string utilities")
{
  SECTION("lower string")
  {
    REQUIRE( wic::toLowerString("Foo Bar 1") == "foo bar 1" );
    REQUIRE( wic::toLowerString("FoO BaR 2") == "foo bar 2" );
    REQUIRE( wic::toLowerString("FOO BAR 3") == "foo bar 3" );
    REQUIRE( wic::toLowerString("fOo bAr 4") == "foo bar 4" );
  }

  SECTION("trim leading spaces")
  {
    std::string str("     foo bar");
    wic::trim(str);
    REQUIRE( str == "foo bar" );
  }

  SECTION("trim leading tabs")
  {
    std::string str(" \t \t \t foo bar");
    wic::trim(str);
    REQUIRE( str == "foo bar" );
  }

  SECTION("trim trailing spaces")
  {
    std::string str("foo bar     ");
    wic::trim(str);
    REQUIRE( str == "foo bar" );
  }

  SECTION("trim trailing tabs")
  {
    std::string str("foo bar \t \t \t ");
    wic::trim(str);
    REQUIRE( str == "foo bar" );
  }

  SECTION("trim leading and trailing characters")
  {
    std::string str(" \t \t \t foo bar \t \t \t ");
    wic::trim(str);
    REQUIRE( str == "foo bar" );
  }
}
