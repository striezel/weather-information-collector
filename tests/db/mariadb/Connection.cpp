/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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
#include <iostream>
#include <catch.hpp>
#include "../../../src/db/mariadb/Connection.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("Connection tests")
{
  using namespace wic;
  const bool isCI = isGitlabCi() || isGithubActions() || isTravisCi();

  // Only run this test, if we are within the CI environment.
  if (isCI)
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

    SECTION("quote method for datetime")
    {
      const db::mariadb::Connection conn(connInfo);

      const auto now = std::chrono::system_clock::now();
      const std::string refString = "'2020-04-01 12:34:56'";
      REQUIRE_NOTHROW( conn.quote(now) );
      REQUIRE( conn.quote(now).size() == refString.size() );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
