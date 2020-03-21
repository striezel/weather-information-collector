/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2019  Dirk Stolle

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
#include "../../../src/util/SemVer.hpp"

TEST_CASE("SemVer class")
{
  using namespace wic;
  SemVer version;

  SECTION("value after construction")
  {
    REQUIRE( version.toString() == std::string("0.0.0") );
  }

  SECTION("constructor with version")
  {
    REQUIRE( SemVer(1, 2, 3).toString() == std::string("1.2.3") );
    REQUIRE( SemVer(0, 9, 1).toString() == std::string("0.9.1") );
  } // constructor section

  SECTION("equality operator")
  {
    SemVer v1, v2;
    REQUIRE( v1 == v2 );

    v1 = SemVer(0, 0, 1);
    REQUIRE_FALSE( v1 == v2 );
    v2 = SemVer(0, 0, 1);
    REQUIRE( v1 == v2 );

    v1 = SemVer(0, 2, 1);
    REQUIRE_FALSE( v1 == v2 );
    v1 = SemVer(2, 0, 1);
    REQUIRE_FALSE( v1 == v2 );
  }

  SECTION("less than operator")
  {
    SemVer v1, v2;
    REQUIRE_FALSE( v1 < v2 );

    v1 = SemVer(0, 0, 1);
    REQUIRE_FALSE( v1 < v2 );
    REQUIRE( v2 < v1 );

    v1 = SemVer(0, 2, 0);
    v2 = SemVer(0, 4, 0);
    REQUIRE( v1 < v2 );
    REQUIRE_FALSE( v2 < v1 );

    v1 = SemVer(2, 3, 1);
    v2 = SemVer(0, 3, 1);
    REQUIRE_FALSE( v1 < v2 );
    REQUIRE( v2 < v1 );
  }
}
