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

#include <iostream>
#include "../../find_catch.hpp"
#include "../../../src/db/mariadb/Connection.hpp"
#include "../../../src/db/mariadb/Result.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("Result class tests")
{
  using namespace wic;
  using namespace wic::db::mariadb;
  const bool hasDB = (isGitlabCi() || isGithubActions()) && !isMinGW();

  // Only run this test, if we have a database server.
  if (hasDB)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );
    REQUIRE( InitDB::createTableApi(connInfo) );
    REQUIRE( InitDB::fillTableApi(connInfo) );

    SECTION("field metadata")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT apiID, name, baseURL FROM api LIMIT 1;");

      REQUIRE( result.good() );

      // There should be three fields.
      REQUIRE( result.fields().size() == 3 );
      // --    1st field: apiID, integer
      REQUIRE( result.fields().at(0).name == "apiID" );
      REQUIRE( result.fields().at(0).type == FieldType::Integer );
      // --    2nd field: name, string
      REQUIRE( result.fields().at(1).name == "name" );
      REQUIRE( result.fields().at(1).type == FieldType::String );
      // --    3rd field: baseURL, string
      REQUIRE( result.fields().at(2).name == "baseURL" );
      REQUIRE( result.fields().at(2).type == FieldType::String );
    }

    SECTION("result data")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT apiID, name, baseURL FROM api WHERE name LIKE '%OpenWeather%' LIMIT 1;");

      REQUIRE( result.good() );

      // There should be one result row.
      REQUIRE( result.rows().size() == 1 );
      // Data: apiID=2, name=OpenWeatherMap, baseURL=https://api.openweathermap.org/...
      REQUIRE( result.rows().at(0).column(0) == "2" );
      REQUIRE( result.rows().at(0).column(1) == "OpenWeatherMap" );
      REQUIRE( result.rows().at(0).column(2) == "https://api.openweathermap.org/data/2.5/" );
    }

    SECTION("hasRows")
    {
      db::mariadb::Connection conn(connInfo);
      const auto result = conn.query("SELECT 1 AS foo FROM DUAL HAVING foo=2;");

      REQUIRE( result.good() );

      // There should be no result rows.
      REQUIRE_FALSE( result.hasRows() );
      REQUIRE( result.rows().size() == 0 );
    }
  }
  else
  {
    std::clog << "Info: Test is run without a database instance, so it is not executed." << std::endl;
  }
}
