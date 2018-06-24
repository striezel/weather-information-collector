/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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
#include "../../src/tasks/Task.hpp"

TEST_CASE("Class Task")
{
  using namespace wic;

  SECTION("constructor")
  {
    Task t;

    REQUIRE_FALSE( t.complete() );
    REQUIRE( t.api() == ApiType::none );
    REQUIRE( t.location().empty() );
    REQUIRE( t.interval() == std::chrono::seconds::zero() );
  }

  SECTION("constructor with parameters")
  {
    Location loc;
    loc.setName("City");
    loc.setId(123456);

    Task t(loc, ApiType::OpenWeatherMap, std::chrono::seconds(3600));

    REQUIRE( t.api() == ApiType::OpenWeatherMap );
    REQUIRE( t.location().name() == "City" );
    REQUIRE( t.location().id() == 123456 );
    REQUIRE( t.interval().count() == 3600 );
  }

  SECTION("complete")
  {
    Location loc;
    loc.setName("City");
    loc.setId(123456);

    Task t;
    REQUIRE_FALSE( t.complete() );

    t = Task(Location(), ApiType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    t = Task(Location(), ApiType::Apixu, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );

    t = Task(Location(), ApiType::Apixu, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    t = Task(loc);
    REQUIRE_FALSE( t.complete() );

    t = Task(loc, ApiType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    t = Task(loc, ApiType::Apixu);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::DarkSky);
    REQUIRE_FALSE( t.complete() );

    t = Task(loc, ApiType::Apixu, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::DarkSky, std::chrono::seconds(36));
    REQUIRE( t.complete() );

    // negative duration shall not count
    t = Task(loc, ApiType::Apixu, std::chrono::seconds(-3600));
    REQUIRE_FALSE( t.complete() );
  }
}
