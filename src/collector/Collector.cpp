/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022  Dirk Stolle

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

#include "Collector.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include "../api/Factory.hpp"
#include "../data/Weather.hpp"
#include "../db/mariadb/StoreMariaDB.hpp"

namespace wic
{

TaskData::TaskData(const Task& t, const std::chrono::steady_clock::time_point& tp)
: task(t),
  nextRequest(tp)
{
}


Collector::Collector()
: tasksContainer(std::vector<TaskData>()),
  apiKeys(std::map<ApiType, std::string>()),
  connInfo(ConnectionInformation("", "", "", "", 0)),
  planWb(PlanWeatherbit::none),
  planWs(PlanWeatherstack::none),
  stopFlag(false)
{
}

bool Collector::fromConfiguration(const Configuration& conf)
{
  // API keys
  apiKeys[ApiType::none] = "";
  apiKeys[ApiType::Apixu] = conf.key(ApiType::Apixu);
  apiKeys[ApiType::OpenWeatherMap] = conf.key(ApiType::OpenWeatherMap);
  apiKeys[ApiType::DarkSky] = conf.key(ApiType::DarkSky);
  apiKeys[ApiType::Weatherbit] = conf.key(ApiType::Weatherbit);
  apiKeys[ApiType::Weatherstack] = conf.key(ApiType::Weatherstack);
  // Open-Meteo has no key, but let's keep it consistent and simplify key access
  // by using the empty value from the configuration.
  apiKeys[ApiType::OpenMeteo] = conf.key(ApiType::OpenMeteo);

  // copy tasks
  const auto now = std::chrono::steady_clock::now();
  tasksContainer.clear();
  for (const auto& t : conf.tasks())
  {
    tasksContainer.emplace_back(t, now);
    const auto api = Factory::create(t.api(), planWb, planWs, "");
    if (api->needsApiKey() && apiKeys[t.api()].empty())
    {
      std::cerr << "Error: API key for " << toString(t.api()) << " is not set, "
                << "but there is a task for that API and it requires a key!\n";
      return false;
    } // if
  } // for
  if (tasksContainer.empty())
  {
    std::cerr << "Error: There are no tasks in the configuration!\n";
    return false;
  }
  // get connection information
  connInfo = conf.connectionInfo();
  if (!connInfo.isComplete())
  {
    std::cerr << "Error: Database connection information is incomplete!\n";
    return false;
  }
  // Get information about pricing plans.
  planWb = conf.planWeatherbit();
  planWs = conf.planWeatherstack();
  return true;
}

int_least32_t Collector::nextIndex() const
{
  if (tasksContainer.empty())
    return -1;

  auto minTime = tasksContainer[0].nextRequest;
  std::vector<TaskData>::size_type minIndex = 0;
  const auto taskLength = tasksContainer.size();
  for (std::vector<TaskData>::size_type i = 1; i < taskLength; ++i)
  {
    if (tasksContainer[i].nextRequest < minTime)
    {
      minTime = tasksContainer[i].nextRequest;
      minIndex = i;
    }
  } // for
  return static_cast<int_least32_t>(minIndex);
}

void Collector::collect()
{
  auto idx = nextIndex();
  if (idx < 0)
    return;

  while (true)
  {
    if (stopRequested())
    {
      return;
    }
    const Location& loc = tasksContainer[idx].task.location();
    const std::string key = apiKeys[tasksContainer[idx].task.api()];
    std::unique_ptr<API> api = Factory::create(tasksContainer[idx].task.api(), planWb, planWs, key);
    if (api == nullptr)
    {
      std::cerr << "Error: Cannot collect data for unsupported API type "
                << toString(tasksContainer[idx].task.api()) << "!" << std::endl;
      return;
    } // if

    // Retrieve data.
    switch (tasksContainer[idx].task.data())
    {
      case DataType::Current:
           collectCurrent(*api, tasksContainer[idx].task.api(), loc);
           break;
      case DataType::Forecast:
           collectForecast(*api, tasksContainer[idx].task.api(), loc);
           break;
      case DataType::CurrentAndForecast:
           collectCurrentAndForecast(*api, tasksContainer[idx].task.api(), loc);
           break;
      default: // i. e. none and possible future unimplemented stuff
           std::cerr << "Error: Request for data type "
                     << toString(tasksContainer[idx].task.data())
                     << " cannot be handled!" << std::endl;
           break;
    } // switch
    api = nullptr;
    // update time for next request
    tasksContainer[idx].nextRequest = tasksContainer[idx].nextRequest + tasksContainer[idx].task.interval();
    // find index for next request
    idx = nextIndex();
    // check stop flag again
    if (stopRequested())
    {
      return;
    }
    // wait until next request is due
    std::this_thread::sleep_until(tasksContainer[idx].nextRequest);
  } // while
}

void Collector::collectCurrent(API& api, const ApiType type, const Location& loc) const
{
  Weather weather;
  if (api.currentWeather(loc, weather))
  {
    StoreMariaDB sql(connInfo);
    if (!sql.saveCurrentWeather(type, loc, weather))
    {
      std::cerr << "Error: Could not save weather data to database!" << std::endl;
    } // if
  } // if
  else
  {
    std::cerr << "Error: Could not get current weather data from API "
              << toString(type) << "!" << std::endl;
  }
}

void Collector::collectForecast(API& api, const ApiType type, const Location& loc) const
{
  Forecast forecast;
  if (api.forecastWeather(loc, forecast))
  {
    StoreMariaDB sql(connInfo);
    if (!sql.saveForecast(type, loc, forecast))
    {
      std::cerr << "Error: Could not save forecast data to database!" << std::endl;
    } // if
  } // if
  else
  {
    std::cerr << "Error: Could not get weather forecast data from API "
              << toString(type) << "!" << std::endl;
  }
}

void Collector::collectCurrentAndForecast(API& api, const ApiType type, const Location& loc) const
{
  Weather weather;
  Forecast forecast;
  if (api.currentAndForecastWeather(loc, weather, forecast))
  {
    StoreMariaDB sql(connInfo);
    if (!sql.saveCurrentWeather(type, loc, weather))
    {
      std::cerr << "Error: Could not save weather data to database!" << std::endl;
    } // if
    if (!sql.saveForecast(type, loc, forecast))
    {
      std::cerr << "Error: Could not save forecast data to database!" << std::endl;
    } // if
  } // if
  else
  {
    std::cerr << "Error: Could not get current weather and forecast data from API "
              << toString(type) << "!" << std::endl;
  }
}

void Collector::stop()
{
  stopFlag = true;
}

bool Collector::stopRequested() const
{
  return stopFlag;
}

} // namespace
