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
#include <mysql++/mysql++.h>
#include "../db/API.hpp"
#include "../db/Structure.hpp"

namespace wic
{

bool UpdateTo090::perform(const ConnectionInformation& ci)
{
  if (!updateStructure(ci))
    return false;
  return updateData(ci);
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
