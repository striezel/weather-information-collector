/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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
#include <mysql++/mysql++.h>
#include "../../third-party/nlohmann/json.hpp"
#include "../db/API.hpp"
#include "../db/Structure.hpp"

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
  query << "SELECT locationID FROM `location` WHERE ISNULL(country_code);";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Error: Could not get locations from table location.\n"
              << "Internal error: " << query.error() << std::endl;
    return false;
  }
  // SELECT was successful, but do we have some data?
  if (result.num_rows() == 0)
  {
    // Nothing to do here.
    std::clog << "Info: All locations already have a country code." << std::endl;
    return true;
  }
  std::vector<uint_least32_t> locations;
  for(uint_least32_t i = 0; i < result.num_rows(); ++i)
  {
    mysqlpp::Query subQuery(&conn);
    subQuery << "SELECT dataID, json FROM weatherdata WHERE apiID = " << owmApiId
             << " AND locationID = " << result[i]["locationID"]
             << " AND json LIKE '%\"country\"%' LIMIT 1;";
    mysqlpp::StoreQueryResult jsonResult = subQuery.store();
    if (jsonResult.num_rows() == 0)
    {
      std::clog << "Info: Found not matching country code for location "
                << result[i]["locationID"] << ". Therefore it will not be "
                << "updated. (This is intentional and not an error.)" << std::endl;
      continue;
    }
    const std::string json = jsonResult.at(0)["json"].c_str();
    // Parse JSON.
    nlohmann::json jsonRoot;
    try
    {
      jsonRoot = nlohmann::json::parse(json);
    }
    catch(...)
    {
      std::cerr << "Error: Unable to parse JSON data of dataID " << jsonResult[i]["dataID"] << "!" << std::endl;
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
      mysqlpp::Query updateQuery(&conn);
      updateQuery << "UPDATE location SET country_code = " << mysqlpp::quote << countryCode
                  << " WHERE locationID = " << result[i]["locationID"] << " LIMIT 1;";
      if (!updateQuery.exec())
      {
        std::cerr << "Error: Could not set country code for location " << result[i]["locationID"]
                  << " to '" << countryCode << "'!" << std::endl;
        return false;
      }
    } // if there is a country code
  } // for

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
  }
  else
  {
    mysqlpp::Connection conn(false);
    if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                      ci.password().c_str(), ci.port()))
    {
      // Should not happen, because previous connection attempts were successful,
      // but better be safe than sorry.
      std::cerr << "Error: Could not connect to database!" << std::endl;
      return false;
    }
    // Create table forecast.
    mysqlpp::Query query(&conn);
    query << "ALTER TABLE `location`  ADD `country_code` CHAR(2) DEFAULT NULL COMMENT 'ISO 3166 ALPHA-2 two letter country code of the location'  AFTER `postcode`;";
    if (!query.exec())
    {
      std::cerr << "Error: Could not add column country_code to table location." << std::endl
                << "Internal error: " << query.error() << std::endl;
      return false;
    }
  } // else (i.e. column has to be added)

  // All queries were successful. (Or there was nothing to do.)
  return true;
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
  query << "INSERT INTO `api` (`name`, `baseURL`) VALUES ('Weatherbit', 'https://api.weatherbit.io/v2.0/');";
  if (!query.exec())
    return false;
  // Done.
  return query.insert_id() > 0;
}

} // namespace
