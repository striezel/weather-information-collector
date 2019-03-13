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
#include "../../../src/tasks/Task.hpp"

TEST_CASE("Class Task")
{
  using namespace wic;

  SECTION("constructor")
  {
    Task t;

    REQUIRE_FALSE( t.complete() );
    REQUIRE( t.api() == ApiType::none );
    REQUIRE( t.data() == DataType::none );
    REQUIRE( t.location().empty() );
    REQUIRE( t.interval() == std::chrono::seconds::zero() );
  }

  SECTION("constructor with parameters")
  {
    Location loc;
    loc.setName("City");
    loc.setId(123456);

    Task t(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600));

    REQUIRE( t.api() == ApiType::OpenWeatherMap );
    REQUIRE( t.data() == DataType::Current );
    REQUIRE( t.location().name() == "City" );
    REQUIRE( t.location().id() == 123456 );
    REQUIRE( t.interval().count() == 3600 );
  }

  SECTION("complete")
  {
    Location loc;
    loc.setName("City");
    loc.setId(123456);

    // empty / default-constructed task is not complete
    Task t;
    REQUIRE_FALSE( t.complete() );

    // task with duration only is not complete (0001 = 1)
    t = Task(Location(), ApiType::none, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    // task with data type only is not complete (0010 = 2)
    t = Task(Location(), ApiType::none, DataType::Current, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::none, DataType::Forecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::none, DataType::CurrentAndForecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );

    // task with data type and duration only is not complete (0011 = 3)
    t = Task(Location(), ApiType::none, DataType::Current, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::none, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::none, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    // task with API type only is not complete (0100 = 4)
    t = Task(Location(), ApiType::Apixu, DataType::none, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::none, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::none, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );

    // task with API type and duration only is not complete (0101 = 5)
    t = Task(Location(), ApiType::Apixu, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    // task with API type and data type only is not complete (0110 = 6)
    t = Task(Location(), ApiType::Apixu, DataType::Current, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::Current, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::Apixu, DataType::Forecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::Forecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::Forecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::Apixu, DataType::CurrentAndForecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::CurrentAndForecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::CurrentAndForecast, std::chrono::seconds::zero());
    REQUIRE_FALSE( t.complete() );

    // task with API type and data type and duration only is not complete (0111 = 7)
    t = Task(Location(), ApiType::Apixu, DataType::Current, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::Current, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::Apixu, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::Apixu, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::OpenWeatherMap, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(Location(), ApiType::DarkSky, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    // task with location only is not complete (1000 = 8)
    t = Task(loc);
    REQUIRE_FALSE( t.complete() );

    // task with location and duration only is not complete (1001 = 9)
    t = Task(loc, ApiType::none, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    // task with location and API only is not complete (1100 = 12)
    t = Task(loc, ApiType::Apixu);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::DarkSky);
    REQUIRE_FALSE( t.complete() );

    // task with location and API and duration only is not complete (1101 = 13)
    t = Task(loc, ApiType::Apixu, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::none, std::chrono::seconds(36));
    REQUIRE_FALSE( t.complete() );

    // task with location and API type and data type and duration only is not complete (1110 = 14)
    t = Task(loc, ApiType::Apixu, DataType::Current);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::Current);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::Current);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::Apixu, DataType::Forecast);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::Forecast);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::Forecast);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::Apixu, DataType::CurrentAndForecast);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::CurrentAndForecast);
    REQUIRE_FALSE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::CurrentAndForecast);
    REQUIRE_FALSE( t.complete() );

    // task with all data is complete (1111 = 15)
    t = Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::Apixu, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::Forecast, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::Apixu, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::OpenWeatherMap, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE( t.complete() );
    t = Task(loc, ApiType::DarkSky, DataType::CurrentAndForecast, std::chrono::seconds(36));
    REQUIRE( t.complete() );

    // negative duration shall not count
    t = Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(-3600));
    REQUIRE_FALSE( t.complete() );
  }
}
