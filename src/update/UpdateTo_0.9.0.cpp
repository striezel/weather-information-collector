/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2020  Dirk Stolle

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

#include "UpdateTo_0.9.0.hpp"
#include <iostream>
#include "../../third-party/nlohmann/json.hpp"
#include "../db/mariadb/API.hpp"
#include "../db/mariadb/Result.hpp"
#include "../db/mariadb/Structure.hpp"

namespace wic
{

bool setCountryCodes(const ConnectionInformation& ci)
{
  const int owmApiId = db::API::getId(ci, ApiType::OpenWeatherMap);
  if (owmApiId <= 0)
  {
    // Nothing to do here.
    std::cerr << "Error: API entry for OpenWeatherMap is missing!" << std::endl;
    return false;
  }

  try
  {
    db::mariadb::Connection conn(ci);
    const auto result = conn.query("SELECT locationID FROM `location` WHERE ISNULL(country_code);");
    if (!result.good())
    {
      std::cerr << "Error: Could not get locations from table location.\n"
                << "Internal error: " << conn.errorInfo() << std::endl;
      return false;
    }
    // SELECT was successful, but do we have some data?
    if (result.rowCount() == 0)
    {
      // Nothing to do here.
      std::clog << "Info: All locations already have a country code." << std::endl;
      return true;
    }

    for(uint_least32_t i = 0; i < result.rowCount(); ++i)
    {
      const std::string subQuery = "SELECT dataID, json FROM weatherdata WHERE apiID = "
             + conn.quote(std::to_string(owmApiId))
             + " AND locationID = " + result.row(i).column(0)
             + " AND json LIKE '%\"country\"%' LIMIT 1;";
      const auto jsonResult = conn.query(subQuery);
      if (!jsonResult.good())
      {
        std::cerr << "Error: Data query for locationID " << result.row(i).column(0)
                  << " failed! " << conn.errorInfo() << std::endl;
        return false;
      }
      if (jsonResult.rowCount() == 0)
      {
        std::clog << "Info: Found no matching country code for location "
                  << result.row(i).column(0) << ". Therefore it will not be "
                  << "updated. (This is intentional and not an error.)" << std::endl;
        continue;
      }
      const std::string json = jsonResult.row(0).column(jsonResult.fieldIndex("json"));
      // Parse JSON.
      nlohmann::json jsonRoot;
      try
      {
        jsonRoot = nlohmann::json::parse(json);
      }
      catch(...)
      {
        std::cerr << "Error: Unable to parse JSON data of dataID " << jsonResult.row(0).column(jsonResult.fieldIndex("dataID")) << "!" << std::endl;
        return false;
      }
      auto sys = jsonRoot.find("sys");
      if (sys == jsonRoot.end() || !sys->is_object())
      {
        continue;
      }
      auto country = sys->find("country");
      if (country != sys->end() && country->is_string())
      {
        const std::string countryCode = country->get<std::string>();
        const std::string updateQuery = "UPDATE location SET country_code = "
                    + conn.quote(countryCode)
                    + " WHERE locationID = " + result.row(i).column(result.fieldIndex("locationID"))
                    + " LIMIT 1;";
        if (conn.exec(updateQuery) <= 0)
        {
          std::cerr << "Error: Could not set country code for location "
                    << result.row(i).column(result.fieldIndex("locationID"))
                    << " to '" << countryCode << "'!" << std::endl;
          return false;
        }
      } // if there is a country code
    } // for
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }

  return true;
}

bool UpdateTo090::perform(const ConnectionInformation& ci)
{
  if (!updateStructure(ci))
    return false;
  if (!updateData(ci))
    return false;
  return setCountryCodes(ci);
}

bool UpdateTo090::updateStructure(const ConnectionInformation& ci)
{
  if (Structure::columnExists(ci, "location", "country_code"))
  {
    std::clog << "Info: Column country_code already exists in table location." << std::endl;
    return true;
  }

  // Column has to be added.
  try
  {
    db::mariadb::Connection conn(ci);
    const std::string sql = "ALTER TABLE `location`  ADD `country_code` CHAR(2) DEFAULT NULL COMMENT 'ISO 3166 ALPHA-2 two letter country code of the location'  AFTER `postcode`;";
    if (conn.exec(sql) < 0)
    {
      std::cerr << "Error: Could not add column country_code to table location." << std::endl
                << "Internal error: " << conn.errorInfo() << std::endl;
      return false;
    }

    // All queries were successful.
    return true;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

bool UpdateTo090::updateData(const ConnectionInformation& ci)
{
  const int id = db::API::getId(ci, ApiType::Weatherbit);
  if (id > 0)
  {
    // Nothing to do here.
    std::cout << "Info: API entry for Weatherbit already exists." << std::endl;
    return true;
  }

  try
  {
    db::mariadb::Connection conn(ci);
    const std::string sql = "INSERT INTO `api` (`name`, `baseURL`) VALUES ('Weatherbit', 'https://api.weatherbit.io/v2.0/');";
    if (conn.exec(sql) < 0)
    {
      std::cerr << "Error: Could not insert Weatherbit into API table. "
                << "Internal error: " << conn.errorInfo() << std::endl;
      return false;
    }

    // Done.
    return conn.lastInsertId() > 0;
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!"
              << "Internal error: " << ex.what() << std::endl;
    return false;
  }
}

} // namespace
