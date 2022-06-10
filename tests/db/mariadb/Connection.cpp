/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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

#include <chrono>
#include <ctime>
#include <iostream>
#include "../../find_catch.hpp"
#include "../../../src/db/mariadb/Connection.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("Connection tests")
{
  using namespace wic;
  const bool hasDB = (isGitlabCi() || isGithubActions()) && !isMinGW();

  // Only run this test, if we have a database server.
  if (hasDB)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );

    SECTION("connection established")
    {
      const db::mariadb::Connection conn(connInfo);

      REQUIRE_FALSE( nullptr == conn.raw() );
    }

    SECTION("escape method")
    {
      const db::mariadb::Connection conn(connInfo);

      REQUIRE( conn.escape("") == "" );
      REQUIRE( conn.escape("foo bar") == "foo bar" );
      REQUIRE( conn.escape("123.45") == "123.45" );
      REQUIRE( conn.escape("2020-04-01 12:34:56") == "2020-04-01 12:34:56" );
      REQUIRE( conn.escape("foo'bar") == "foo\\'bar" );
      REQUIRE( conn.escape("line\r\nbreak") == "line\\r\\nbreak" );
      REQUIRE( conn.escape(std::string("NUL\0byte", 8)) == "NUL\\0byte" );
    }

    SECTION("quote method for strings")
    {
      const db::mariadb::Connection conn(connInfo);

      REQUIRE( conn.quote("") == "''" );
      REQUIRE( conn.quote("foo bar") == "'foo bar'" );
      REQUIRE( conn.quote("123.45") == "'123.45'" );
      REQUIRE( conn.quote("2020-04-01 12:34:56") == "'2020-04-01 12:34:56'" );
      REQUIRE( conn.quote("foo'bar") == "'foo\\'bar'" );
      REQUIRE( conn.quote("line\r\nbreak") == "'line\\r\\nbreak'" );
      REQUIRE( conn.quote(std::string("NUL\0byte", 8)) == "'NUL\\0byte'" );
    }
  }
  else
  {
    std::clog << "Info: Test is run without a database instance, so it is not executed." << std::endl;
  }
}

TEST_CASE("Connection::quote(time_point) tests")
{
  using namespace wic::db::mariadb;

  SECTION("quote method for datetime")
  {
    const auto now = std::chrono::system_clock::now();
    const std::string refString = "'2020-04-01 12:34:56'";
    REQUIRE_NOTHROW( Connection::quote(now) );
    REQUIRE( Connection::quote(now).size() == refString.size() );
  }

  SECTION("time with leading zeroes")
  {
    std::tm tm{};
    tm.tm_year = 1990 - 1900; // 1990
    tm.tm_mon = 5 - 1; // May
    tm.tm_mday = 1; // 1st
    tm.tm_hour = 6;
    tm.tm_min = 7;
    tm.tm_sec = 8;
    tm.tm_isdst = -1; // no info on daylight saving time
    const std::time_t tt = std::mktime(&tm);
    REQUIRE( tt != static_cast<std::time_t>(-1) );

    const auto date_time = std::chrono::system_clock::from_time_t(tt);
    const auto date_time_string = Connection::quote(date_time);
    REQUIRE_FALSE( date_time_string.empty() );
    REQUIRE( date_time_string == "'1990-05-01 06:07:08'" );
  }

  SECTION("time without leading zeroes")
  {
    std::tm tm{};
    tm.tm_year = 2030 - 1900; // 2030
    tm.tm_mon = 12 - 1; // December
    tm.tm_mday = 24; // 24th
    tm.tm_hour = 19;
    tm.tm_min = 20;
    tm.tm_sec = 35;
    tm.tm_isdst = -1; // no info on daylight saving time
    const std::time_t tt = std::mktime(&tm);
    REQUIRE( tt != static_cast<std::time_t>(-1) );

    const auto date_time = std::chrono::system_clock::from_time_t(tt);
    const auto date_time_string = Connection::quote(date_time);
    REQUIRE_FALSE( date_time_string.empty() );
    REQUIRE( date_time_string == "'2030-12-24 19:20:35'" );
  }
}
