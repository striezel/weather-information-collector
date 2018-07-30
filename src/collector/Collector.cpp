/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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

#include "Collector.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include "../api/API.hpp"
#include "../api/Apixu.hpp"
#include "../api/DarkSky.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../data/Weather.hpp"
#include "../store/StoreMySQL.hpp"

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

  if (apiKeys[ApiType::Apixu].empty()
      && apiKeys[ApiType::OpenWeatherMap].empty()
      && apiKeys[ApiType::DarkSky].empty())
  {
    std::cerr << "Error: No API keys are set!\n";
    return false;
  }
  // copy tasks
  const auto now = std::chrono::steady_clock::now();
  tasksContainer.clear();
  for (const auto& t : conf.tasks())
  {
    tasksContainer.push_back(TaskData(t, now));
    if (apiKeys[t.api()].empty())
    {
      std::cerr << "Error: API key for " << toString(t.api()) << " is not set, "
                << "but there is a task for that API!\n";
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
  return true;
}

int_least32_t Collector::nextIndex() const
{
  if (tasksContainer.empty())
    return -1;

  auto minTime = tasksContainer[0].nextRequest;
  int_least32_t minIndex = 0;
  const auto taskLength = tasksContainer.size();
  for (std::vector<TaskData>::size_type i = 1; i < taskLength; ++i)
  {
    if (tasksContainer[i].nextRequest < minTime)
    {
      minTime = tasksContainer[i].nextRequest;
      minIndex = i;
    }
  } // for
  return minIndex;
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
    std::unique_ptr<API> api = nullptr;
    const std::string key = apiKeys[tasksContainer[idx].task.api()];
    switch (tasksContainer[idx].task.api())
    {
      case ApiType::OpenWeatherMap:
           api.reset(new wic::OpenWeatherMap(key));
           break;
      case ApiType::Apixu:
           api.reset(new wic::Apixu(key));
           break;
      case ApiType::DarkSky:
           api.reset(new wic::DarkSky(key));
           break;
      case ApiType::none:
      default:
           std::cerr << "Error: Cannot collect data for unsupported API type "
                     << toString(tasksContainer[idx].task.api()) << "!" << std::endl;
           return;
    } // switch

    // Retrieve data.
    switch (tasksContainer[idx].task.data())
    {
      case DataType::Current:
           {
             Weather weather;
             if (api->currentWeather(loc, weather))
             {
               StoreMySQL sql(connInfo);
               if (!sql.saveCurrentWeather(tasksContainer[idx].task.api(), loc, weather))
               {
                 std::cerr << "Error: Could not save weather data to database!" << std::endl;
               } // if
             } // if
             else
             {
               std::cerr << "Error: Could not get current weather data from API "
                         << toString(tasksContainer[idx].task.api()) << "!" << std::endl;
             }
           }
           break;
      case DataType::Forecast:
           {
             Forecast forecast;
             if (api->forecastWeather(loc, forecast))
             {
               // TODO: Save data.
               if (true)
               {
                 std::cerr << "Error: Could not save weather forecast data to database!" << std::endl;
               } // if
             } // if
             else
             {
               std::cerr << "Error: Could not get weather forecast data from API "
                         << toString(tasksContainer[idx].task.api()) << "!" << std::endl;
             }
           }
           break;
      case DataType::CurrentAndForecast:
      case DataType::none:
      default:
           std::cerr << "Error: Request for data type "
                     << toString(tasksContainer[idx].task.data())
                     << " cannot be handled yet!" << std::endl;
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

void Collector::stop()
{
  stopFlag = true;
}

bool Collector::stopRequested() const
{
  return stopFlag;
}

} // namespace
