/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2022  Dirk Stolle

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
#include "../../../src/util/Strings.hpp"

TEST_CASE("string utilities")
{
  using namespace wic;

  SECTION("lower string")
  {
    REQUIRE( toLowerString("Foo Bar 1") == "foo bar 1" );
    REQUIRE( toLowerString("FoO BaR 2") == "foo bar 2" );
    REQUIRE( toLowerString("FOO BAR 3") == "foo bar 3" );
    REQUIRE( toLowerString("fOo bAr 4") == "foo bar 4" );
  }

  SECTION("trim")
  {
    SECTION("empty string")
    {
      std::string s;
      trim(s);

      REQUIRE( s.empty() );
    }

    SECTION("trim leading spaces")
    {
      std::string str("     foo bar");
      trim(str);
      REQUIRE( str == "foo bar" );
    }

    SECTION("trim leading tabs")
    {
      std::string str(" \t \t \t foo bar");
      trim(str);
      REQUIRE( str == "foo bar" );
    }

    SECTION("trim trailing spaces")
    {
      std::string str("foo bar     ");
      trim(str);
      REQUIRE( str == "foo bar" );
    }

    SECTION("trim trailing tabs")
    {
      std::string str("foo bar \t \t \t ");
      trim(str);
      REQUIRE( str == "foo bar" );
    }

    SECTION("trim leading and trailing characters")
    {
      std::string str(" \t \t \t foo bar \t \t \t ");
      trim(str);
      REQUIRE( str == "foo bar" );
    }

    SECTION("string contains only spaces and tabs")
    {
      std::string s{" \t\t    \t"};
      trim(s);
      REQUIRE( s.empty() );
    }
  }

  SECTION("stringToInt")
  {
    SECTION("empty string")
    {
      int i{0};
      REQUIRE_FALSE( stringToInt("", i) );
    }

    SECTION("string with invalid characters")
    {
      int i{0};
      REQUIRE_FALSE( stringToInt("1fa2", i) );
    }

    SECTION("non-negative numbers")
    {
      int i{-10};

      REQUIRE( stringToInt("0", i) );
      REQUIRE( i == 0 );

      REQUIRE( stringToInt("1", i) );
      REQUIRE( i == 1 );

      REQUIRE( stringToInt("19", i) );
      REQUIRE( i == 19 );

      REQUIRE( stringToInt("32767", i) );
      REQUIRE( i == 32767 );
    }

    SECTION("negative numbers")
    {
      int i{-10};

      REQUIRE( stringToInt("-1", i) );
      REQUIRE( i == -1 );

      REQUIRE( stringToInt("-123", i) );
      REQUIRE( i == -123 );

      REQUIRE( stringToInt("-2037", i) );
      REQUIRE( i == -2037 );

      REQUIRE( stringToInt("-32765", i) );
      REQUIRE( i == -32765 );
    }

    SECTION("(near) maximum integer values")
    {
      int i{42};
      // test for 16 bit int type
      if constexpr (std::numeric_limits<int>::max() == 32767)
      {
        REQUIRE( stringToInt("32767", i) );
        REQUIRE( i == 32767 );

        REQUIRE( stringToInt("-32767", i) );
        REQUIRE( i == -32767 );

        REQUIRE_FALSE( stringToInt("32768", i) );
        REQUIRE_FALSE( stringToInt("327600", i) );
      }
      // test for 32 bit int type
      else if constexpr (std::numeric_limits<int>::max() == 2147483647L)
      {
        REQUIRE( stringToInt("2147483647", i) );
        REQUIRE( i == 2147483647 );

        REQUIRE( stringToInt("-2147483647", i) );
        REQUIRE( i == -2147483647 );

        REQUIRE_FALSE( stringToInt("2147483648", i) );
        REQUIRE_FALSE( stringToInt("21474836400", i) );
      }
      // test for 64 bit int type
      else if constexpr (std::numeric_limits<int>::max() == 9223372036854775807LL)
      {
        REQUIRE( stringToInt("9223372036854775807", i) );
        REQUIRE( i == 9223372036854775807 );

        REQUIRE( stringToInt("-9223372036854775807", i) );
        REQUIRE( i == -9223372036854775807 );

        REQUIRE_FALSE( stringToInt("9223372036854775808", i) );
        REQUIRE_FALSE( stringToInt("92233720368547758000", i) );
      }
      else
      {
        REQUIRE("Error: There is no matching test for this integer type." == std::string("yes"));
      }
    }
  }

  SECTION("floatToString")
  {
    REQUIRE( floatToString(0.0f) == "0" );
    REQUIRE( floatToString(std::numeric_limits<float>::quiet_NaN()) == "nan" );

    REQUIRE( floatToString(0.1f) == "0.1" );
    REQUIRE( floatToString(1.25f) == "1.25" );
    REQUIRE( floatToString(123.45f) == "123.45" );
    REQUIRE( floatToString(12345.5f) == "12345.5" );

    REQUIRE( floatToString(-0.2f) == "-0.2" );
    REQUIRE( floatToString(-81.375f) == "-81.375" );
    REQUIRE( floatToString(-225.0f) == "-225" );
    REQUIRE( floatToString(-54321.6f) == "-54321.6" );
  }

  SECTION("stringToFloat")
  {
    SECTION("empty string")
    {
      float f{0.0f};
      REQUIRE_FALSE( stringToFloat("", f) );
    }

    SECTION("string with invalid characters")
    {
      float f{0.0f};
      REQUIRE_FALSE( stringToFloat("1a2.3", f) );
      REQUIRE_FALSE( stringToFloat("1.2a3", f) );
    }

    SECTION("non-negative numbers")
    {
      float f{0.1f};

      REQUIRE( stringToFloat("0", f) );
      REQUIRE( f == 0.0f );

      REQUIRE( stringToFloat("12.75", f) );
      REQUIRE( f == 12.75 );

      REQUIRE( stringToFloat("19", f) );
      REQUIRE( f == 19.0f );

      REQUIRE( stringToFloat("1234567890.125", f) );
      REQUIRE( f == 1234567890.125f );
    }

    SECTION("negative numbers")
    {
      float f{-10};

      REQUIRE( stringToFloat("-1", f) );
      REQUIRE( f == -1.0f );

      REQUIRE( stringToFloat("-1.25", f) );
      REQUIRE( f == -1.25f );

      REQUIRE( stringToFloat("-203.75", f) );
      REQUIRE( f == -203.75f );

      REQUIRE( stringToFloat("-543210.5", f) );
      REQUIRE( f == -543210.5f );
    }
  }
}
