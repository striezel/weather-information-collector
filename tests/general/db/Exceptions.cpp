/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2022  Dirk Stolle

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
#include "../../../src/db/Exceptions.hpp"

TEST_CASE("db-related exceptions")
{
  using namespace wic::db;

  SECTION("AllocationFailure")
  {
    AllocationFailure ex;
    REQUIRE( ex.what() == std::string("Error: Could not allocate database handle!") );
  }

  SECTION("ConnectionFailure")
  {
    const std::string message("Something went awfully wrong, dear chaps, but there is nothing we can do about it.");
    ConnectionFailure ex(message);

    REQUIRE( ex.what() == message );
  }
}
