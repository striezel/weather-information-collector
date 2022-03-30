/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2018, 2019, 2020  Dirk Stolle

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
#include <catch.hpp>
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

    SECTION("five tasks with same location and data type but different API")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(900)));
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


    SECTION("Apixu + OpenWeatherMap + DarkSky + Weatherbit + Weatherstack: five tasks within limits")
    {
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(900)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(9000)));

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

    SECTION("Apixu + OpenWeatherMap + DarkSky + Weatherbit + Weatherstack: out of limit, even if only one API has too much requests")
    {
      // too much OWM requests, but Apixu + DarkSky + Weatherbit + Weatherstack requests within limit
      for (int i = 1; i <= 61; ++i)
      {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much Apixu requests, but OWM + DarkSky + Weatherbit + Weatherstack requests within limit
      tasks.clear();
      for (int i = 1; i <= 14; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much DarkSky requests, but OWM + Apixu + Weatherbit + Weatherstack requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much Weatherbit requests, but OWM + Apixu + DarkSky + Weatherstack requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(7200)));
      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Free, PlanWeatherbit::Free, PlanWeatherstack::Free) );

      // too much Weatherstack requests, but OWM + Apixu + DarkSky + Weatherbit requests within limit
      tasks.clear();
      for (int i = 1; i <= 50; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(3600)));
      }
      tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Apixu, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::DarkSky, DataType::Current, std::chrono::seconds(3600)));
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(3600)));
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

    SECTION("missing '=' separator")
    {
      const auto data = "location.id123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-missing-separator.conf";
      {
        std::ofstream file(name);
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
        std::ofstream file(name);
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
        std::ofstream file(name);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }

    SECTION("multiple API values")
    {
      const auto data = "location.id=123\nlocation.name=Hammelburg\nlocation.coordinates=50.1,9.8\nlocation.postcode=97762\napi=OpenWeatherMap\napi=OpenWeatherMap\ninterval=900"sv;
      const auto name = "loadFromFile-multiple-apis.conf";
      {
        std::ofstream file(name);
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
        std::ofstream file(name);
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
        std::ofstream file(name);
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
        std::ofstream file(name);
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
        std::ofstream file(name);
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
        std::ofstream file(name);
        file.write(data.data(), data.size());
        file.close();
      }

      REQUIRE_FALSE( TaskManager::loadFromFile(name, task) );
      REQUIRE( std::remove(name) == 0 );
    }
  }
}
