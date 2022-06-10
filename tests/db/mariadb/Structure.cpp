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
#include "../../../src/db/mariadb/Structure.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("Structure tests")
{
  using namespace wic;
  const bool isCI = isGitlabCi() || isGithubActions() || isTravisCi();

  // Only run this test, if we are within the CI environment.
  if (isCI)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );
    REQUIRE( InitDB::createTableApi(connInfo) );

    SECTION("check for existing tables")
    {
      REQUIRE( Structure::tableExists(connInfo, "api") );
      REQUIRE_FALSE( Structure::tableExists(connInfo, "does_not_exist_in_db") );
    }

    SECTION("check for existing columns")
    {
      // existing columns in one existing table
      REQUIRE( Structure::columnExists(connInfo, "api", "apiID") );
      REQUIRE( Structure::columnExists(connInfo, "api", "name") );
      // non-existing column in one existing table
      REQUIRE_FALSE( Structure::columnExists(connInfo, "api", "foobar") );
      // non-existing column in non-existing table
      REQUIRE_FALSE( Structure::columnExists(connInfo, "foo", "bar") );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
