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
#include "../../src/tasks/TaskManager.hpp"

TEST_CASE("Class TaskManager")
{
  /* Note:

     The tests for loading configuration files are part of a separate test
     application. It can be found in the manger/ subdirectory.
  */

  using namespace wic;

  Location loc;
  loc.setName("London");
  loc.setId(123456789);

  SECTION("hasDuplicates")
  {
    std::vector<Task> tasks;

    Location loc2;
    loc.setName("Paris");
    loc.setId(987654321);

    SECTION("empty task list")
    {
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks) );
    }

    SECTION("three tasks with same location but different API")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(900)));
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks) );
    }

    SECTION("two tasks with same API but different location")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(900)));
      tasks.push_back(Task(loc2, ApiType::OpenWeatherMap, std::chrono::seconds(900)));
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks) );
    }

    SECTION("two tasks with same API, same location, but different interval")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(1801)));
      REQUIRE( TaskManager::hasDuplicates(tasks) );
    }
  }


  SECTION("withinLimit")
  {
    std::vector<Task> tasks;

    SECTION("empty task list")
    {
      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("API none is never within limits")
    {
      tasks.push_back(Task(loc, ApiType::none, std::chrono::seconds(3600)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Interval zero (or less) is never within limits")
    {
      tasks.push_back(Task(loc, ApiType::none, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::none, std::chrono::seconds(-3)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }

    SECTION("OpenWeatherMap only: two tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(900)));

      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("OpenWeatherMap only: two tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(1)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(1)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }

    SECTION("OpenWeatherMap only: multitude of tasks within limits")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(60)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(2)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("OpenWeatherMap only: multitude of tasks with too much requests")
    {
      for (int i = 1; i <= 61; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }


    SECTION("Apixu only: two tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(1800)));
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(1800)));

      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Apixu only: two tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(900)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Apixu only: multitude of tasks within limits")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(3600)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(1200)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Apixu only: multitude of tasks with too much requests")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }


    SECTION("DarkSky only: two tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(1800)));
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(1800)));

      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("DarkSky only: two tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(120)));
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(120)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }

    SECTION("DarkSky only: multitude of tasks within limits")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(3600)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(180)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("DarkSky only: multitude of tasks with too much requests")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }


    SECTION("Apixu + OpenWeatherMap + DarkSky: three tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(900)));

      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Apixu + OpenWeatherMap: few tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(60)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(1)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(1)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Apixu + OpenWeatherMap: multitude of tasks within limits")
    {
      for (int i = 1; i <= 30; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(60)));
      }
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(2400)));
      }
      REQUIRE( TaskManager::withinLimits(tasks) );
    }

    SECTION("Apixu + OpenWeatherMap + DarkSky: out of limit, even if only one API has too much requests")
    {
      // too much OWM requests, but Apixu + DarkSky requests within limit
      for (int i = 1; i <= 61; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(1)));
      }
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(3600)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      // too much Apixu requests, but OWM + DarkSky requests within limit
      tasks.clear();
      for (int i = 1; i <= 7; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(3600)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );

      // too much DarkSky requests, but OWM + Apixu requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, std::chrono::seconds(3600)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks) );
    }
  } //withinLimits
}
