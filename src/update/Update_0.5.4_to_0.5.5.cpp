/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017  Dirk Stolle

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

#include "Update_0.5.4_to_0.5.5.hpp"
#include <map>
#include <mysql++/mysql++.h>
#include "../api/API.hpp"
#include "../api/Apixu.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../api/Types.hpp"
#include "../db/API.hpp"
#include "../db/Structure.hpp"

namespace wic
{

bool Update054_055::perform(const ConnectionInformation& ci)
{
  if (!updateStructure(ci))
    return false;
  return updateData(ci);
}

bool Update054_055::updateStructure(const ConnectionInformation& ci)
{
  if (!Structure::tableExists(ci, "weatherdata"))
  {
    std::cerr << "Error: Table weatherdata does not exist in database "
              << ci.db() << "! Update cannot be performed." << std::endl;
    return false;
  }
  //If column already exists, structure is already up to date.
  if (Structure::columnExists(ci, "weatherdata", "rain"))
    return true;
  //Add new column rain.
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    //Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "ALTER TABLE `weatherdata`  ADD `rain` FLOAT NULL DEFAULT NULL COMMENT 'amount of rain in millimeters'  AFTER `humidity`;";
  if (!query.exec())
  {
    std::cerr << "Error: Could not add new column \"rain\" to table weatherdata.\n";
    return false;
  }
  //ALTER TABLE was successful.
  return true;
}

bool Update054_055::updateData(const ConnectionInformation& ci)
{
  std::map<int, ApiType> id_to_type;

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
  } //for

  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    //Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT dataID, apiID, json FROM weatherdata WHERE ISNULL(rain) AND NOT ISNULL(json);";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const auto rows = result.num_rows();
  if (rows == 0)
  {
    //Nothing to do here.
    return true;
  }

  for (unsigned long int i = 0; i < rows; ++i)
  {
    const int apiID = result[i]["apiID"];
    const auto iter = id_to_type.find(apiID);
    if (iter == id_to_type.end())
    {
      //Probably a newer API type, so skip it.
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
           //Newer or unsupported API, go on.
           continue;
           break;
    } //switch
    Weather w;
    const unsigned int dataId = result[i]["dataID"];
    if (!api->parseCurrentWeather(result[i]["json"].c_str(), w))
    {
      std::cerr << "Error: Could not parse JSON data for data ID " << dataId
                << "!" << std::endl;
      return false;
    }
    if (w.hasRain())
    {
      mysqlpp::Query update(&conn);
      update << "UPDATE weatherdata SET rain=" << mysqlpp::quote << w.rain()
             << " WHERE dataID=" << mysqlpp::quote << dataId << " LIMIT 1;";
      if (!update.exec())
      {
        std::cerr << "Error: Could not update rain amount for data ID "
                  << dataId << "!" << std::endl;
        return false;
      }
      std::clog << "Info: Updated rain amount for data ID " << dataId << " to "
                << w.rain() << " mm." << std::endl;
    } //if data set has rain data
  } //for
  //Done.
  return true;
}

} //namespace
