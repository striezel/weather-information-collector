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
#include "../../../src/db/mysqlpp/Utilities.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../CiData.hpp"
#include "InitDB.hpp"

TEST_CASE("Utilities tests")
{
  using namespace wic;
  const bool isCI = isGitlabCi();

  // Only run this test, if we are within the CI environment.
  if (isCI)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );
    REQUIRE( InitDB::createTableLocation(connInfo) );

    const Location Dresden = CiData::getDresden();
    const Location Tiksi = CiData::getTiksi();

    SECTION("one location")
    {
      mysqlpp::Connection conn(false);
      REQUIRE( conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                            connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()));

      const int idDresden = getLocationId(conn, Dresden);
      REQUIRE_FALSE( idDresden == -1 );
      // Getting location again shall return same id.
      const int idDresdenAgain = getLocationId(conn, Dresden);
      REQUIRE( idDresden == idDresdenAgain );
    }

    SECTION("two locations")
    {
      mysqlpp::Connection conn(false);
      REQUIRE( conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                            connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()));

      const int idDresden = getLocationId(conn, Dresden);
      REQUIRE_FALSE( idDresden == -1 );

      const int idTiksi = getLocationId(conn, Tiksi);
      REQUIRE_FALSE( idTiksi == -1 );

      // New location should have different id.
      REQUIRE( idDresden != idTiksi );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
