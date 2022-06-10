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
#include "../../../src/db/mariadb/guess.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("database version guessing tests")
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
    REQUIRE( InitDB::createTableLocation(connInfo) );
    REQUIRE( InitDB::createTableWeatherData(connInfo) );
    REQUIRE( InitDB::createTableForecast(connInfo) );
    REQUIRE( InitDB::createTableForecastData(connInfo) );

    SECTION("guess current version for up-to-date database")
    {
      const auto guessedVersion = guessVersionFromDatabase(connInfo);
      // Database should be the most up to date version.
      REQUIRE( guessedVersion == mostUpToDateVersion );
    }


    SECTION("guess version for 0.8.6 database")
    {
      REQUIRE( InitDB::downgradeTo_0_8_6(connInfo) );
      const auto guessedVersion = guessVersionFromDatabase(connInfo);
      // Database should be the version 0.8.6.
      REQUIRE( guessedVersion == SemVer(0, 8, 6) );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
