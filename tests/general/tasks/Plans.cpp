/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2022, 2025  Dirk Stolle

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
    const auto expert = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Expert, PlanWeatherbit::none, PlanWeatherstack::none);
    const auto enterprise = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::Enterprise, PlanWeatherbit::none, PlanWeatherstack::none);

    REQUIRE( free.requests < startup.requests );
    REQUIRE( free.timespan == startup.timespan );

    REQUIRE( startup.requests < dev.requests );
    REQUIRE( startup.timespan == dev.timespan );

    REQUIRE( dev.requests < prof.requests );
    REQUIRE( dev.timespan == prof.timespan );

    REQUIRE( prof.requests < expert.requests );
    REQUIRE( prof.timespan == expert.timespan );

    REQUIRE( expert.requests < enterprise.requests );
    REQUIRE( expert.timespan == enterprise.timespan );
  }

  SECTION("OpenWeatherMap plan 'none' returns limit with zero allowed requests")
  {
    const auto nothing = Limits::forApi(ApiType::OpenWeatherMap, PlanOwm::none, PlanWeatherbit::none, PlanWeatherstack::none);
    REQUIRE( nothing.requests == 0 );
  }

  SECTION("Weatherbit plans have different limits")
  {
    const auto free = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Free, PlanWeatherstack::none);
    const auto standard = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Standard, PlanWeatherstack::none);
    const auto plus = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Plus, PlanWeatherstack::none);
    const auto business = Limits::forApi(ApiType::Weatherbit, PlanOwm::none, PlanWeatherbit::Business, PlanWeatherstack::none);

    REQUIRE( free.requests < standard.requests );
    REQUIRE( free.timespan == standard.timespan );

    REQUIRE( standard.requests < plus.requests );
    REQUIRE( standard.timespan == plus.timespan );

    REQUIRE( plus.requests < business.requests );
    REQUIRE( plus.timespan == business.timespan );
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

    SECTION("several tasks within limits of expert plan")
    {
      for (int i = 0; i < 160; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::Expert, PlanWeatherbit::none, PlanWeatherstack::none) );
    }

    SECTION("several tasks with too much requests for expert plan")
    {
      for (int i = 0; i < 1670; ++i) {
        tasks.push_back(Task(loc, ApiType::OpenWeatherMap, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::Expert, PlanWeatherbit::none, PlanWeatherstack::none) );
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
    SECTION("tasks within limits of standard plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(4)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Standard, PlanWeatherstack::none) );
    }

    SECTION("tasks with too much requests for standard plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(2)));

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Standard, PlanWeatherstack::none) );
    }

    SECTION("tasks within limits of plus plan")
    {
      tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Plus, PlanWeatherstack::none) );
    }

    SECTION("tasks with too much requests for plus plan")
    {
      for (int i = 0; i < 3; ++i) {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Plus, PlanWeatherstack::none) );
    }

    SECTION("tasks within limits of business plan")
    {
      for (int i = 0; i < 6; ++i) {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Business, PlanWeatherstack::none) );
    }

    SECTION("tasks with too much requests for business plan")
    {
      for (int i = 0; i < 25; ++i) {
        tasks.push_back(Task(loc, ApiType::Weatherbit, DataType::Current, std::chrono::seconds(1)));
      }

      REQUIRE_FALSE( TaskManager::withinLimits(tasks, PlanOwm::none, PlanWeatherbit::Business, PlanWeatherstack::none) );
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
