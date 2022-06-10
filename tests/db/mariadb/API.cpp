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
#include "../../../src/db/mariadb/API.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../InitDB.hpp"

TEST_CASE("Get API ids")
{
  using namespace wic;
  const bool hasDB = (isGitlabCi() || isGithubActions()) && !isMinGW();

  // Only run this test, if we have a database server.
  if (hasDB)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );
    REQUIRE( InitDB::createTableApi(connInfo) );

    SECTION("query filled api table")
    {
      REQUIRE( InitDB::fillTableApi(connInfo) );

      SECTION("id of Apixu")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::Apixu);

        REQUIRE( id != -1 );
        REQUIRE( id == 1 );
      }

      SECTION("id of OpenWeatherMap")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::OpenWeatherMap);

        REQUIRE( id != -1 );
        REQUIRE( id == 2 );
      }

      SECTION("id of DarkSky")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::DarkSky);

        REQUIRE( id != -1 );
        REQUIRE( id == 3 );
      }

      SECTION("id of Weatherbit")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::Weatherbit);

        REQUIRE( id != -1 );
        REQUIRE( id == 4 );
      }

      SECTION("id of Weatherstack")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::Weatherstack);

        REQUIRE( id != -1 );
        REQUIRE( id == 5 );
      }
    }

    SECTION("query empty api table")
    {
      REQUIRE( InitDB::truncateTableApi(connInfo) );

      SECTION("id of Apixu")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::Apixu);

        REQUIRE( id == 0 );
      }

      SECTION("id of OpenWeatherMap")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::OpenWeatherMap);

        REQUIRE( id == 0 );
      }

      SECTION("id of DarkSky")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::DarkSky);

        REQUIRE( id == 0 );
      }

      SECTION("id of Weatherbit")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::Weatherbit);

        REQUIRE( id == 0 );
      }

      SECTION("id of Weatherstack")
      {
        const int id = wic::db::API::getId(connInfo, ApiType::Weatherstack);

        REQUIRE( id == 0 );
      }
    }
  }
  else
  {
    std::clog << "Info: Test is run without database instance, so it is not executed." << std::endl;
  }
}
