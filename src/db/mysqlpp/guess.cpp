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

#include "guess.hpp"
#include <iostream>
#include <mysql++/mysql++.h>
#include "Structure.hpp"

namespace wic
{

SemVer guessVersionFromDatabase(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return SemVer();
  }

  // 0.9.0: Column `country_code` was added to location.
  if (Structure::columnExists(ci, "location", "country_code"))
  {
    return SemVer(0, 9, 0);
  }

  // 0.8.6: cloudiness is nullable.
  {
    mysqlpp::Query query(&conn);
    query << "SELECT IS_NULLABLE FROM information_schema.columns "
          << "  WHERE TABLE_SCHEMA=" << mysqlpp::quote << ci.db()
          << "    AND TABLE_NAME='weatherdata' AND COLUMN_NAME='cloudiness';";
    mysqlpp::StoreQueryResult result = query.store();
    if (!result)
    {
      std::cerr << "Error: Could not get column information of column \"cloudiness\" in table weatherdata.\n"
                << "Internal error: " << query.error() << std::endl;
      return SemVer();
    }
    // SELECT was successful, but do we have some data?
    if (result.num_rows() == 0)
    {
      std::cerr << "Error: Result of column information query is empty!" << std::endl;
      return SemVer();
    }
    const std::string isNullable = result.at(0)["IS_NULLABLE"].c_str();
    if (isNullable == "YES")
    {
      return SemVer(0, 8, 6);
    }
  }

  // 0.8.3: Column `snow` was added to weatherdata and forecastdata.
  // If column already exists, database is at least from version 0.8.3.
  if (Structure::columnExists(ci, "weatherdata", "snow"))
  {
    return SemVer(0, 8, 3);
  }

  // 0.7.0: Tables for forecasts (forecast and forecastdata) were added.
  if (Structure::tableExists(ci, "forecast") && Structure::tableExists(ci, "forecastdata"))
  {
    return SemVer(0, 7, 0);
  }

  // 0.6.6: Column `json` in the table `weatherdata` changed from TEXT to MEDIUMTEXT.
  {
    mysqlpp::Query query(&conn);
    query << "SELECT LOWER(DATA_TYPE) AS jdt FROM information_schema.COLUMNS "
          << "WHERE TABLE_NAME='weatherdata' AND COLUMN_NAME='json' "
          << "  AND TABLE_SCHEMA=" << mysqlpp::quote << ci.db() << " LIMIT 1;";
    mysqlpp::StoreQueryResult result = query.store();
    if (!result)
    {
      std::cerr << "Error: Could not get data type of column \"json\" in table weatherdata.\n"
                << "Internal error: " << query.error() << std::endl;
      return SemVer();
    }
    if (result.num_rows() == 0)
    {
      std::cerr << "Error: Result of data type query is empty!" << std::endl;
      return SemVer();
    }
    const std::string type = result.at(0)["jdt"].c_str();
    if (type == "mediumtext")
      return SemVer(0, 6, 6);
  }

  // 0.5.5: Column `rain` was added to weatherdata table.
  if (Structure::columnExists(ci, "weatherdata", "rain"))
    return SemVer(0, 5, 5);

  // No other indications - must be very early, so set it to 0.0.1. ("Ancient".)
  // That version never existed, but it is earlier than every known version, so
  // all updates should be applied.
  return ancientVersion;
}

} // namespace
