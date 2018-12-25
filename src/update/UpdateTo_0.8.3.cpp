/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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

#include "UpdateTo_0.8.3.hpp"
#include <algorithm>
#include <chrono>
#include <map>
#include <thread>
#include <mysql++/mysql++.h>
#include "../api/API.hpp"
#include "../api/Apixu.hpp"
#include "../api/DarkSky.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../api/Types.hpp"
#include "../db/API.hpp"
#include "../db/Structure.hpp"
#include "ForecastDataUpdate_0.8.3.hpp"
#include "WeatherDataUpdate_0.8.3.hpp"

namespace wic
{

unsigned int determineThreadNumber()
{
  const unsigned int concurrent = std::thread::hardware_concurrency();
  unsigned int threadsOfExecution = 4;
  if (concurrent >= 16)
    threadsOfExecution = 32;
  if (concurrent >= 12)
    threadsOfExecution = 24;
  else if (concurrent >= 8)
    threadsOfExecution = 16;
  else if ((1 <= concurrent) && (concurrent < 4))
    threadsOfExecution = 2;
  return threadsOfExecution;
}

bool UpdateTo083::perform(const ConnectionInformation& ci)
{
  if (!updateStructure(ci))
    return false;
  return updateData(ci);
}

bool UpdateTo083::updateStructure(const ConnectionInformation& ci)
{
  if (!Structure::tableExists(ci, "forecastdata"))
  {
    std::cerr << "Error: Table forecastdata does not exist in database "
              << ci.db() << "! Update cannot be performed." << std::endl;
    return false;
  }
  // If column does not exist, create it.
  if (!Structure::columnExists(ci, "forecastdata", "snow"))
  {
    // Add new column snow.
    const auto start = std::chrono::steady_clock::now();
    mysqlpp::Connection conn(false);
    if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                      ci.password().c_str(), ci.port()))
    {
      // Should not happen, because previous connection attempts were successful,
      // but better be safe than sorry.
      std::cerr << "Error: Could not connect to database!" << std::endl;
      return false;
    }
    mysqlpp::Query query(&conn);
    query << "ALTER TABLE `forecastdata`  ADD `snow` FLOAT NULL DEFAULT NULL COMMENT 'amount of snow in millimeters'  AFTER `rain`;";
    if (!query.exec())
    {
      std::cerr << "Error: Could not add new column \"snow\" to table forecastdata.\n";
      return false;
    }
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "  Info: Structure of table forecastdata has been updated."
              << " Took " << duration.count() << " milliseconds." << std::endl;
  }
  else
  {
    std::cout << "  Info: Structure of table forecastdata already seems to be up to date." << std::endl;
  }

  if (!Structure::tableExists(ci, "weatherdata"))
  {
    std::cerr << "Error: Table weatherdata does not exist in database "
              << ci.db() << "! Update cannot be performed." << std::endl;
    return false;
  }
  // If column already exists, structure is already up to date.
  if (Structure::columnExists(ci, "weatherdata", "snow"))
  {
    std::cout << "  Info: Structure of table weatherdata already seems to be up to date." << std::endl;
    return true;
  }

  // Add new column snow.
  const auto start = std::chrono::steady_clock::now();
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "ALTER TABLE `weatherdata`  ADD `snow` FLOAT NULL DEFAULT NULL COMMENT 'amount of snow in millimeters'  AFTER `rain`;";
  if (!query.exec())
  {
    std::cerr << "Error: Could not add new column \"snow\" to table weatherdata.\n";
    return false;
  }
  const auto end = std::chrono::steady_clock::now();
  const std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // ALTER TABLE was successful.
  std::cout << "  Info: Structure of table weatherdata has been updated."
            << " Took " << duration.count() << " milliseconds." << std::endl;
  return true;
}

bool UpdateTo083::updateData(const ConnectionInformation& ci)
{
  // Update weather data first.
  if (!updateWeatherData(ci))
    return false;
  // forecast data
  return updateForecastData(ci);
}

bool UpdateTo083::updateWeatherData(const ConnectionInformation& ci)
{
  std::map<int, ApiType> id_to_type;
  for (const ApiType type : { ApiType::Apixu, ApiType::DarkSky, ApiType::OpenWeatherMap})
  {
    const int id = db::API::getId(ci, type);
    if (id <= 0)
    {
      std::cerr << "Error: Could not get ID for API type " << toString(type)
                << "!" << std::endl;
      return false;
    }
    id_to_type[id] = type;
  } // for

  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT dataID, apiID, json FROM weatherdata WHERE ISNULL(snow) AND NOT ISNULL(json);";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const auto rows = result.num_rows();
  if (rows == 0)
  {
    // Nothing to do here.
    return true;
  }

  const unsigned int threadsOfExecution = determineThreadNumber();
  std::cout << "  Info: Using " << threadsOfExecution << " thread(s) to update"
            << " snow data in weatherdata. This may take a while." << std::endl;

  const auto start = std::chrono::steady_clock::now();
  std::vector<WeatherDataUpdate_083> updates;
  for (unsigned int i = 0; i < threadsOfExecution; ++i)
  {
    updates.push_back(WeatherDataUpdate_083(ci));
  }

  std::vector<unsigned int> startIndices;
  for (unsigned int i = 0; i < threadsOfExecution; ++i)
  {
    startIndices.push_back(std::min(i * rows / threadsOfExecution, rows -1));
  }
  // End index of last thread.
  startIndices.push_back(rows - 1);

  std::vector<std::thread> threads;
  for (unsigned int i = 0; i < threadsOfExecution; ++i)
  {
    threads.push_back(std::thread(updates[i], startIndices[i], startIndices[i+1], id_to_type, result));
  }

  // Join all threads.
  for (auto& t: threads)
  {
    t.join();
  }

  // Check for failures.
  for (const WeatherDataUpdate_083& upd: updates)
  {
    if (upd.failed())
    {
      return false;
    }
  }
  const auto end = std::chrono::steady_clock::now();
  const std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "  Info: Data of table weatherdata (" << rows
            << " rows) has been updated where data was available. Took "
            << duration.count() << " milliseconds." << std::endl;

  // Done.
  return true;
}

bool UpdateTo083::updateForecastData(const ConnectionInformation& ci)
{
  std::map<int, ApiType> id_to_type;
  for (const ApiType type : { ApiType::Apixu, ApiType::DarkSky, ApiType::OpenWeatherMap})
  {
    const int id = db::API::getId(ci, type);
    if (id <= 0)
    {
      std::cerr << "Error: Could not get ID for API type " << toString(type)
                << "!" << std::endl;
      return false;
    }
    id_to_type[id] = type;
  } // for

  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT apiID, dataID, dataTime, json FROM forecastdata"
        << " LEFT JOIN forecast ON forecastdata.forecastID=forecast.forecastID"
        << " WHERE ISNULL(snow) AND NOT ISNULL(json);";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const auto rows = result.num_rows();
  if (rows == 0)
  {
    // Nothing to do here.
    return true;
  }

  const unsigned int threadsOfExecution = determineThreadNumber();
  std::cout << "  Info: Using " << threadsOfExecution << " thread(s) to update"
            << " snow data in forecastdata. This may take a while." << std::endl;

  const auto start = std::chrono::steady_clock::now();
  std::vector<ForecastDataUpdate_083> updates;
  for (unsigned int i = 0; i < threadsOfExecution; ++i)
  {
    updates.push_back(ForecastDataUpdate_083(ci));
  }

  std::vector<unsigned int> startIndices;
  for (unsigned int i = 0; i < threadsOfExecution; ++i)
  {
    startIndices.push_back(std::min(i * rows / threadsOfExecution, rows -1));
  }
  // End index of last thread.
  startIndices.push_back(rows - 1);

  std::vector<std::thread> threads;
  for (unsigned int i = 0; i < threadsOfExecution; ++i)
  {
    threads.push_back(std::thread(updates[i], startIndices[i], startIndices[i+1], id_to_type, result));
  }

  // Join all threads.
  for (auto& t: threads)
  {
    t.join();
  }

  // Check for failures.
  for (const ForecastDataUpdate_083& upd: updates)
  {
    if (upd.failed())
    {
      return false;
    }
  }
  const auto end = std::chrono::steady_clock::now();
  const std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "  Info: Data of table forecastdata (" << rows
            << " rows) has been updated where data was available. Took "
            << duration.count() << " milliseconds." << std::endl;

  // Done.
  return true;
}

} // namespace
