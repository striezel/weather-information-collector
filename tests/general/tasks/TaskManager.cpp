/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2022  Dirk Stolle

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

#include <cstdio> // for std::remove
#include <fstream>
#include <string_view>
#include "../../find_catch.hpp"
#include "../../../src/tasks/TaskManager.hpp"

TEST_CASE("Class TaskManager")
{
  /* Note:

     The tests for loading configuration files are part of a separate test
     application. It can be found in the task-manager/ subdirectory.
  */

  using namespace wic;

  Location loc;
  loc.setName("London");
  loc.setOwmId(123456789);

  SECTION("hasDuplicates")
  {
    std::vector<Task> tasks;

    Location loc2;
    loc.setName("Paris");
    loc.setOwmId(987654321);

    SECTION("empty task list")
    {
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks, false) );
    }

    SECTION("six tasks with same location and data type but different API")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(900)));
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks, false) );
    }

    SECTION("two tasks with same API and data type but different location")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc2, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks, false) );
    }

    SECTION("three tasks with same API, same location, same interval, but different data type")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Forecast, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::CurrentAndForecast, std::chrono::seconds(900)));
      REQUIRE_FALSE( TaskManager::hasDuplicates(tasks, false) );
    }

    SECTION("two tasks with same API, same data type, same location, but different interval")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1801)));
      REQUIRE( TaskManager::hasDuplicates(tasks, false) );
    }
  }


  SECTION("withinLimit")
  {
    std::vector<Task> tasks;

    SECTION("empty task list")
    {
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("API none is never within limits")
    {
      tasks.push_back(Task(loc, ApiType::none, DataType::Current, std::chrono::seconds(3600)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Interval zero (or less) is never within limits")
    {
      tasks.push_back(Task(loc, ApiType::none, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::none, DataType::Current, std::chrono::seconds(-3)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(0)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(-5)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("OpenWeatherMap only: two tasks within limits of free plan")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("OpenWeatherMap only: two tasks with too much requests for free plan")
    {
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("OpenWeatherMap only: multitude of tasks within limits of free plan")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(2)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("OpenWeatherMap only: multitude of tasks with too much requests for free plan")
    {
      for (int i = 1; i <= 61; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }


    SECTION("Apixu only: two tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(900)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Apixu only: two tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(450)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(450)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Apixu only: multitude of tasks within limits")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(1200)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Apixu only: multitude of tasks with too much requests")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }


    SECTION("DarkSky only: two tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(1800)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(1800)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("DarkSky only: two tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(120)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(120)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("DarkSky only: multitude of tasks within limits")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(180)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("DarkSky only: multitude of tasks with too much requests")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }


    SECTION("Weatherbit only: two tasks within limits of free plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1800)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1800)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Weatherbit only: two tasks with too much requests for free plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(240)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(240)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Weatherbit only: multitude of tasks within limits of free plan")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(360)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Weatherbit only: multitude of tasks with too much requests for free plan")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }


    SECTION("Weatherstack only: two tasks within limits of free plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Weatherstack only: two tasks with too much requests for free plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(120)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(120)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Weatherstack only: multitude of tasks within limits of free plan")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(18000)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Weatherstack only: multitude of tasks with too much requests for free plan")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }


    SECTION("DarkSky only: two tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(1800)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(1800)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("DarkSky only: two tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(120)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(120)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("OpenMeteo only: multitude of tasks within limits")
    {
      for (int i = 1; i <= 6; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(3600)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      tasks.clear();
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(180)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("OpenMeteo only: multitude of tasks with too much requests")
    {
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(60)));
      }
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }


    SECTION("Apixu + OpenWeatherMap + DarkSky + Weatherbit + Weatherstack + OpenMeteo: six tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(9000)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(900)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Apixu + OpenWeatherMap: few tasks with too much requests")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(60)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Apixu + OpenWeatherMap: multitude of tasks within limits")
    {
      for (int i = 1; i <= 30; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(60)));
      }
      for (int i = 1; i <= 2; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(2400)));
      }
      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }

    SECTION("Apixu + OpenWeatherMap + DarkSky + Weatherbit + Weatherstack + OpenMeteo: out of limit, even if only one API has too much requests")
    {
      // too much OWM requests, but Apixu + DarkSky + Weatherbit + Weatherstack + OpenMeteo requests within limit
      for (int i = 1; i <= 61; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much Apixu requests, but OWM + DarkSky + Weatherbit + Weatherstack + OpenMeteo requests within limit
      tasks.clear();
      for (int i = 1; i <= 14; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much DarkSky requests, but OWM + Apixu + Weatherbit + Weatherstack + OpenMeteo requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much Weatherbit requests, but OWM + Apixu + DarkSky + Weatherstack + OpenMeteo requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much Weatherstack requests, but OWM + Apixu + DarkSky + Weatherbit + OpenMeteo requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much OpenMeteo requests, but OWM + Apixu + DarkSky + Weatherbit + Weatherstack requests within limit
      tasks.clear();
      for (int i = 1; i <= 60; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenMeteo, DataType::Current, std::chrono::seconds(30)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );
    }
  } // withinLimits

  SECTION("loadFromFile")
  {
    using namespace std::literals::string_view_literals;

    Task task;

    SECTION("file does not exist")
    {
      REQUIRE_FALSE( TaskManager::loadFromFile("/file/does-not/ex.ist", task) );
    }

    SECTION("carriage return at end of line")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\r\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-carriage-return.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
      REQUIRE( task.location().name() == "Hammelburg" );
    }

    SECTION("carriage return + line feed at end of file")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900\r\n\r\n"sv;
      const auto name = "loadFromFile-carriage-return-at-end-of-file.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
      REQUIRE( task.interval() == std::chrono::seconds(900) );
    }

    SECTION("carriage return at end of every line")
    {
      const auto data = "location.id=123\r\nlocation.name=Hammelburg\r\nlocation.coordinates=50.1,9.8\r\nlocation.postcode=97762\r\napi=OpenWeatherMap\r\ninterval=900"sv;
      const auto name = "loadFromFile-carriage-return-everywhere.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
      REQUIRE( task.location().owmId() == 123 );
      REQUIRE( task.location().name() == "Hammelburg" );
      REQUIRE( task.location().latitude() == 50.1f );
      REQUIRE( task.location().longitude() == 9.8f );
      REQUIRE( task.location().postcode() == "97762" );
      REQUIRE( task.api() == ApiType::OpenWeatherMap );
      REQUIRE( task.interval() == std::chrono::seconds(900) );
    }

    SECTION("missing '=' separator")
    {
      const auto data = "location.id123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-missing-separator.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("empty value")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=\ninterval=900"sv;
      const auto name = "loadFromFile-empty-value.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("unknown setting name")
    {
      const auto data = "blahblubbar=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-unknown-setting-name.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("unknown API value")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=whatisthis\ninterval=900"sv;
      const auto name = "loadFromFile-unknown-api.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("task using Apixu API")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=Apixu\ninterval=900"sv;
      const auto name = "loadFromFile-apixu-api.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
      REQUIRE( task.api() == ApiType::Apixu );
      // Note: This should print a deprecation warning.
    }

    SECTION("multiple API values")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-apis.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple data types")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900\ndata=current\ndata=current"sv;
      const auto name = "loadFromFile-multiple-data-types.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("unknown data type")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900\ndata=thisisnotvalid\n"sv;
      const auto name = "loadFromFile-unknown-data-type.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple location ids")
    {
      const auto data = "location.id=123\nlocation.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-ids.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location id is zero")
    {
      const auto data = "location.id=0\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-id-is-zero.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location id is negative")
    {
      const auto data = "location.id=-123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-id-is-negative.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location id is not an integer")
    {
      const auto data = "location.id=blah\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-id-is-not-int.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple location names")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.name=Hamburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-names.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("very long location name")
    {
      const auto data = "location.id=123\nlocation.name=HammelburgerJohann Gambolputty Der Müßiggang berappen das ausgemergelt Mettigel. Klabusterbeere und Flickschusterei stagnieren fidel Kummerspeck. Mettigel und Jungfer grämen hold Naseweis. Das Zeche anschwärzen das piesacken Fracksausen.\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-long-name.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple location postcodes")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\nlocation.postcode=97761\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-postcodes.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("very long location postcode")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762123456789012345678901223334444\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-long-postcode.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple location country codes")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.countrycode=DE\nlocation.countrycode=DE\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-country-codes.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location country code is longer than two characters")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.countrycode=DEU\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-three-letter-country-code.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location country code is shorter than two characters")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.countrycode=D\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-one-letter-country-code.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple location coordinates")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-coordinates.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("wrong separator in location coordinates")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1/9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-wrong-coordinates-separator.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location coordinates: latitude is not a float")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=abc,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-coordinates-latitude-not-float.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location coordinates: longitude is not a float")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=9.8,abc\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-coordinates-longitude-not-float.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location coordinates: latitude is out of range (> 90)")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=92.5,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-coordinates-latitude-out-of-range.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location coordinates: latitude is out of range (< -90)")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=-92.5,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-coordinates-latitude-out-of-range-negative.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location coordinates: longitude is out of range (> 180)")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=52.5,190.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-coordinates-longitude-out-of-range.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("location coordinates: longitude is out of range (< -180)")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=52.5,-185.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-coordinates-longitude-out-of-range-negative.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple interval values")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-intervals.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("negative interval value")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=-900"sv;
      const auto name = "loadFromFile-negative-interval.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("non-integral interval value")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=9.55"sv;
      const auto name = "loadFromFile-float-interval.conf";
      {
        std::ofstream file(name, std::ios_base::out | std::ios_base::binary);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }
  }
}
