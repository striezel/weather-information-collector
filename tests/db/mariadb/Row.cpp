/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020  Dirk Stolle

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

#include <iostream>
#include <catch.hpp>
#include "../../../src/db/mariadb/Connection.hpp"
#include "../../../src/db/mariadb/Result.hpp"
#include "../../../src/db/mariadb/Row.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("Row class tests")
{
  using namespace wic;
  using namespace wic::db::mariadb;
  const bool isCI = isGitlabCi() || isTravisCi();

  // Only run this test, if we are within the CI environment.
  if (isCI)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );
    REQUIRE( InitDB::createTableApi(connInfo) );
    REQUIRE( InitDB::fillTableApi(connInfo) );

    SECTION("isNull checks")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT 0/0, 0.0/0.0, 123.45, 'foo', -42, '';");

      REQUIRE( result.good() );
      // There should be one row.
      REQUIRE( result.rows().size() == 1 );

      // First two columns should be NULL, MySQL converts div. by zero to NULL.
      REQUIRE( result.rows().at(0).isNull(0) );
      REQUIRE( result.rows().at(0).isNull(1) );
      // Other columns should not be NULL.
      REQUIRE_FALSE( result.rows().at(0).isNull(2) );
      REQUIRE_FALSE( result.rows().at(0).isNull(3) );
      REQUIRE_FALSE( result.rows().at(0).isNull(4) );
      REQUIRE_FALSE( result.rows().at(0).isNull(5) );
    }

    SECTION("getInt checks")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT -42, 32001, 123.45, 'foo', '2020-04-20 12:34:56';");

      REQUIRE( result.good() );

      const auto& row = result.rows().at(0);
      // First and second column should convert to integer.
      REQUIRE( row.getInt(0) == -42 );
      REQUIRE( row.getInt(1) == 32001 );
      // Third column to last column should not convert.
      REQUIRE_THROWS_AS( row.getInt(2), std::invalid_argument);
      REQUIRE_THROWS_AS( row.getInt(3), std::invalid_argument);
      REQUIRE_THROWS_AS( row.getInt(4), std::invalid_argument);
    }

    SECTION("getFloat checks")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT -42, 32001, 123.45, 'foo', '2020-04-20 12:34:56';");

      REQUIRE( result.good() );

      const auto& row = result.rows().at(0);
      // First, second and third column should convert to float.
      REQUIRE( row.getFloat(0) == -42.0f );
      REQUIRE( row.getFloat(1) == 32001.0f );
      REQUIRE( row.getFloat(2) == Approx(123.45f) );
      // Fourth and fifth column should not convert.
      REQUIRE_THROWS_AS( row.getFloat(3), std::invalid_argument);
      REQUIRE_THROWS_AS( row.getFloat(4), std::invalid_argument);
    }

    SECTION("getDouble checks")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT -42, 32001, 123.45, 'foo', '2020-04-20 12:34:56';");

      REQUIRE( result.good() );

      const auto& row = result.rows().at(0);
      // First, second and third column should convert to double.
      REQUIRE( row.getDouble(0) == -42.0 );
      REQUIRE( row.getDouble(1) == 32001.0 );
      REQUIRE( row.getDouble(2) == Approx(123.45) );
      // Fourth and fifth column should not convert.
      REQUIRE_THROWS_AS( row.getDouble(3), std::invalid_argument);
      REQUIRE_THROWS_AS( row.getDouble(4), std::invalid_argument);
    }

    SECTION("getDateTime checks")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT 32001, 'foo', '2020-04-20 12:34:56', NOW();");

      REQUIRE( result.good() );

      const auto& row = result.rows().at(0);
      REQUIRE_THROWS_AS( row.getDateTime(0), std::invalid_argument);
      REQUIRE_THROWS_AS( row.getDateTime(1), std::invalid_argument);
      REQUIRE_NOTHROW( row.getDateTime(2) );
      REQUIRE_NOTHROW( row.getDateTime(3) );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
