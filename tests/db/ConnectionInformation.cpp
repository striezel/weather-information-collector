/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017  Dirk Stolle

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
#include "../../src/db/ConnectionInformation.hpp"

TEST_CASE("ConnectionInformation class for db")
{
  using namespace wic;

  SECTION("default constructor")
  {
    ConnectionInformation connInfo;

    REQUIRE( connInfo.hostname() == "localhost" );
    REQUIRE( connInfo.db() == "database" );
    REQUIRE( connInfo.user() == "root" );
    REQUIRE( connInfo.password() == "" );
    REQUIRE( connInfo.port() == ConnectionInformation::defaultMySqlPort );
  }

  SECTION("constructor with parameters")
  {
    ConnectionInformation connInfo("db-server", "db_one", "foo", "bar", 3307);

    REQUIRE( connInfo.hostname() == "db-server" );
    REQUIRE( connInfo.db() == "db_one" );
    REQUIRE( connInfo.user() == "foo" );
    REQUIRE( connInfo.password() == "bar" );
    REQUIRE( connInfo.port() == 3307 );
  }

  SECTION("constructor with parameters, but w/o port")
  {
    ConnectionInformation connInfo("db-server", "db_one", "foo", "bar");

    REQUIRE( connInfo.hostname() == "db-server" );
    REQUIRE( connInfo.db() == "db_one" );
    REQUIRE( connInfo.user() == "foo" );
    REQUIRE( connInfo.password() == "bar" );
    REQUIRE( connInfo.port() == 3306 );
  }

  SECTION("test for isComplete(): positive examples")
  {
    ConnectionInformation connInfo;
    REQUIRE( connInfo.isComplete() );

    connInfo = ConnectionInformation("db-server", "db_one", "foo", "bar", 3307);
    REQUIRE( connInfo.isComplete() );

    connInfo = ConnectionInformation("db-server", "db_one", "foo", "bar");
    REQUIRE( connInfo.isComplete() );
    //empty password - still complete
    connInfo = ConnectionInformation("db-server", "db_one", "foo", "", 3307);
    REQUIRE( connInfo.isComplete() );
  }

  SECTION("test for isComplete(): negative examples")
  {
    ConnectionInformation connInfo;

    connInfo = ConnectionInformation("", "", "", "", 0);
    REQUIRE_FALSE( connInfo.isComplete() );

    connInfo = ConnectionInformation("", "db_one", "foo", "bar", 3307);
    REQUIRE_FALSE( connInfo.isComplete() );

    connInfo = ConnectionInformation("db-server", "", "foo", "bar", 3307);
    REQUIRE_FALSE( connInfo.isComplete() );

    connInfo = ConnectionInformation("db-server", "db_one", "", "bar", 3307);
    REQUIRE_FALSE( connInfo.isComplete() );

    connInfo = ConnectionInformation("db-server", "db_one", "foo", "", 3307);
    //Empty password may be allowed.
    REQUIRE( connInfo.isComplete() );

    connInfo = ConnectionInformation("db-server", "db_one", "foo", "bar", 0);
    REQUIRE_FALSE( connInfo.isComplete() );
  }

  SECTION("clear")
  {
    ConnectionInformation connInfo("db-server", "db_one", "foo", "bar", 3307);

    connInfo.clear();
    REQUIRE( connInfo.hostname().empty() );
    REQUIRE( connInfo.db().empty() );
    REQUIRE( connInfo.user().empty() );
    REQUIRE( connInfo.password().empty() );
    REQUIRE( connInfo.port() == 0 );
  }

}
