/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2022  Dirk Stolle

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
#include "../../../src/api/Limits.hpp"
#include "../../../src/tasks/TaskManager.hpp"

TEST_CASE("Different API plans")
{
  using namespace wic;

  SECTION("OpenWeatherMap plans have different limits")
  {
    const auto free = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Free, PlanWeatherbit::none, PlanWeatherstack::none);
    const auto startup = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Startup, PlanWeatherbit::none, PlanWeatherstack::none);
    const auto dev = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Developer, PlanWeatherbit::none, PlanWeatherstack::none);
    const auto prof = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Professional, PlanWeatherbit::none, PlanWeatherstack::none);
    const auto enterprise = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Enterprise, PlanWeatherbit::none, PlanWeatherstack::none);

    REQUIRE( free.requests < startup.requests );
    REQUIRE( free.timespan == startup.timespan );

    REQUIRE( startup.requests < dev.requests );
    REQUIRE( startup.timespan == dev.timespan );

    REQUIRE( dev.requests < prof.requests );
    REQUIRE( dev.timespan == prof.timespan );

    REQUIRE( prof.requests < enterprise.requests );
    REQUIRE( prof.timespan == enterprise.timespan );
  }

  SECTION("OpenWeatherMap plan 'none' returns limit with zero allowed requests")
  {
    const auto nothing = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::none);
    REQUIRE( nothing.requests == 0 );
  }

  SECTION("Weatherbit plans have different limits")
  {
    const auto free = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Free, PlanWeatherstack::none);
    const auto starter = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Starter, PlanWeatherstack::none);
    const auto dev = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Developer, PlanWeatherstack::none);
    const auto advanced = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Advanced, PlanWeatherstack::none);

    REQUIRE( free.requests < starter.requests );
    REQUIRE( free.timespan == starter.timespan );

    REQUIRE( starter.requests < dev.requests );
    REQUIRE( starter.timespan == dev.timespan );

    REQUIRE( dev.requests < advanced.requests );
    REQUIRE( dev.timespan == advanced.timespan );
  }

  SECTION("Weatherbit plan 'none' returns limit with zero allowed requests")
  {
    const auto nothing = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::none);
    REQUIRE( nothing.requests == 0 );
  }

  SECTION("Weatherstack plans have different limits")
  {
    const auto free = Limits::forApi(ApiType::Weatherstack, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Free);
    const auto standard = Limits::forApi(ApiType::Weatherstack, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Standard);
    const auto prof = Limits::forApi(ApiType::Weatherstack, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Professional);
    const auto business = Limits::forApi(ApiType::Weatherstack, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Business);

    REQUIRE( free.requests < standard.requests );
    REQUIRE( free.timespan == standard.timespan );

    REQUIRE( standard.requests < prof.requests );
    REQUIRE( standard.timespan == prof.timespan );

    REQUIRE( prof.requests < business.requests );
    REQUIRE( prof.timespan == business.timespan );
  }

  SECTION("Weatherstack plan 'none' returns limit with zero allowed requests")
  {
    const auto nothing = Limits::forApi(ApiType::Weatherstack, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::none);
    REQUIRE( nothing.requests == 0 );
  }
}

TEST_CASE("witinLimit for various plans")
{
  using namespace wic;

  std::vector<Task> tasks;

  Location loc;
  loc.setName("London");
  loc.setOwmId(123456789);

  SECTION("withinLimit: OpenWeatherMap")
  {
    SECTION("several tasks within limits of startup plan")
    {
      for (int i = 0; i < 9; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Startup, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks with too much requests for startup plan")
    {
      for (int i = 0; i < 11; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Startup, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks within limits of developer plan")
    {
      for (int i = 0; i < 50; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Developer, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks with too much requests for developer plan")
    {
      for (int i = 0; i < 51; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Developer, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks within limits of professional plan")
    {
      for (int i = 0; i < 51; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Professional, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks with too much requests for professional plan")
    {
      for (int i = 0; i < 501; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Professional, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks within limits of enterprise plan")
    {
      for (int i = 0; i < 501; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Enterprise, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks with too much requests for enterprise plan")
    {
      for (int i = 0; i < 3334; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Enterprise, PlanWeatherbit::none, PlanWeatherstack::none) );
    }
  } // withinLimits: OpenWeatherMap

  SECTION("withinLimit: Weatherbit")
  {
    SECTION("tasks within limits of starter plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(2)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Starter, PlanWeatherstack::none) );
    }

    SECTION("tasks with too much requests for starter plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Starter, PlanWeatherstack::none) );
    }

    SECTION("tasks within limits of developer plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Developer, PlanWeatherstack::none) );
    }

    SECTION("tasks with too much requests for developer plan")
    {
      for (int i = 0; i < 6; ++i) {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Developer, PlanWeatherstack::none) );
    }

    SECTION("tasks within limits of advanced plan")
    {
      for (int i = 0; i < 6; ++i) {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Advanced, PlanWeatherstack::none) );
    }

    SECTION("tasks with too much requests for advanced plan")
    {
      for (int i = 0; i < 60; ++i) {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Advanced, PlanWeatherstack::none) );
    }
  } // withinLimits: Weatherbit

  SECTION("withinLimit: Weatherstack")
  {
    SECTION("tasks within limits of standard plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(60)));

      REQUIRE(TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Standard));
    }

    SECTION("tasks with too much requests for standard plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(30)));

      REQUIRE_FALSE(TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Standard));
    }

    SECTION("tasks within limits of professional plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(30)));

      REQUIRE(TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Professional));
    }

    SECTION("tasks with too much requests for professional plan")
    {
      for (int i = 0; i < 8; ++i)
      {
        tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(8)));
      }

      REQUIRE_FALSE(TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Professional));
    }

    SECTION("tasks within limits of business plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(3)));

      REQUIRE(TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Business));
    }

    SECTION("tasks with too much requests for business plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherstack, DataType::Current, std::chrono::seconds(2)));

      REQUIRE_FALSE(TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::Business));
    }
  } // withinLimits: Weatherstack
} // withinLimits
