/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2020  Dirk Stolle

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

#include "UpdateTo_0.5.5.hpp"
#include <iostream>
#include <map>
#include <memory>
#include "../api/API.hpp"
#include "../api/Apixu.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../api/Types.hpp"
#include "../db/mariadb/API.hpp"
#include "../db/mariadb/Result.hpp"
#include "../db/mariadb/Structure.hpp"

namespace wic
{

bool UpdateTo055::perform(const ConnectionInformation& ci)
{
  if (!updateStructure(ci))
    return false;
  return updateData(ci);
}

bool UpdateTo055::updateStructure(const ConnectionInformation& ci)
{
  if (!Structure::tableExists(ci, "weatherdata"))
  {
    std::cerr << "Error: Table weatherdata does not exist in database "
              << ci.db() << "! Update cannot be performed." << std::endl;
    return false;
  }
  // If column already exists, structure is already up to date.
  if (Structure::columnExists(ci, "weatherdata", "rain"))
    return true;
  try
  {
    // Add new column rain.
    db::mariadb::Connection conn(ci);
    const auto affected = conn.exec("ALTER TABLE `weatherdata`  ADD `rain` FLOAT NULL DEFAULT NULL COMMENT 'amount of rain in millimeters'  AFTER `humidity`;");
    if (affected < 0)
    {
      std::cerr << "Error: Could not add new column \"rain\" to table weatherdata.\n";
      return false;
    }
    // ALTER TABLE was successful.
    return true;
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

bool UpdateTo055::updateData(const ConnectionInformation& ci)
{
  std::map<int, ApiType> id_to_type;
  // Note: ApiType::DarkSky is not handled here, because that API was not
  // supported in version 0.5.4. DarkSky has only been added in version 0.6.0.
  for (const ApiType type : { ApiType::Apixu, ApiType::OpenWeatherMap})
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

  try
  {
    db::mariadb::Connection conn(ci);
    const auto result = conn.query("SELECT dataID, apiID, json FROM weatherdata WHERE ISNULL(rain) AND NOT ISNULL(json);");
    if (!result.good())
    {
      std::cerr << "Error: Failed to get query result!\n"
                << "Internal error: " << conn.errorInfo() << std::endl;
      return false;
    }
    const auto rows = result.rowCount();
    if (rows == 0)
    {
      // Nothing to do here.
      return true;
    }

    for (unsigned long int i = 0; i < rows; ++i)
    {
      const auto& row = result.row(i);
      const int apiID = row.getInt32(1);
      const auto iter = id_to_type.find(apiID);
      if (iter == id_to_type.end())
      {
        // Probably a newer API type, so skip it.
        continue;
      }
      std::unique_ptr<API> api = nullptr;
      switch (iter->second)
      {
        case ApiType::Apixu:
             api.reset(new wic::Apixu());
             break;
        case ApiType::OpenWeatherMap:
             api.reset(new wic::OpenWeatherMap());
             break;
        default:
             // Newer or unsupported API, go on.
             continue;
             break;
      } // switch
      Weather w;
      const unsigned int dataId = row.getInt64(0);
      if (!api->parseCurrentWeather(row[2].c_str(), w))
      {
        std::cerr << "Error: Could not parse JSON data for data ID " << dataId
                  << "!" << std::endl;
        return false;
      }
      if (w.hasRain())
      {
        const std::string update = "UPDATE weatherdata SET rain=" + conn.quote(std::to_string(w.rain()))
               + " WHERE dataID=" + conn.quote(std::to_string(dataId)) + " LIMIT 1;";
        if (conn.exec(update) < 0)
        {
          std::cerr << "Error: Could not update rain amount for data ID "
                    << dataId << "!" << std::endl;
          return false;
        }
        std::clog << "Info: Updated rain amount for data ID " << dataId << " to "
                  << w.rain() << " mm." << std::endl;
      } // if data set has rain data
    } // for
    // Done.
    return true;
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

} // namespace
