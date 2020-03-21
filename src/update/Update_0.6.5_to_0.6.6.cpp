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

#include "Update_0.6.5_to_0.6.6.hpp"
#include <mysql++/mysql++.h>
#include "../db/mysqlpp/Structure.hpp"

namespace wic
{

bool Update065_066::perform(const ConnectionInformation& ci)
{
  const int rc = updateStructure(ci);
  // If return value is -1, then something went wrong.
  if (rc == -1)
    return false;
  // If return value is zero, then no data update is required.
  if (rc == 0)
    return true;
  return updateData(ci);
}

int Update065_066::updateStructure(const ConnectionInformation& ci)
{
  if (!Structure::tableExists(ci, "weatherdata"))
  {
    std::cerr << "Error: Table weatherdata does not exist in database "
              << ci.db() << "! Update cannot be performed." << std::endl;
    return -1;
  }
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return -1;
  }
  // Get type of column `json` in table `weatherdata`.
  mysqlpp::Query query(&conn);
  query << "SELECT LOWER(DATA_TYPE) AS jdt FROM information_schema.COLUMNS "
        << "WHERE TABLE_NAME='weatherdata' AND COLUMN_NAME='json' "
        << "  AND TABLE_SCHEMA=" << mysqlpp::quote << ci.db() << " LIMIT 1;";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Error: Could not get data type of column \"json\" in table weatherdata.\n"
              << "Internal error: " << query.error() << std::endl;
    return -1;
  }
  // SELECT was successful, but do we have some data?
  if (result.num_rows() == 0)
  {
    std::cerr << "Error: Result of data type query is empty!" << std::endl;
    return -1;
  }
  const std::string type = result.at(0)["jdt"].c_str();
  if (type == "mediumtext")
  {
    // Nothing to do here.
    std::cout << "Info: Table weatherdata already seems to be up to date." << std::endl;
    return 0;
  }
  if (type != "text")
  {
    std::cerr << "Error: column \"json\" in table weatherdata has type \""
              << type << "\", while type \"text\" was expected!" << std::endl;
    return -1;
  }
  query = mysqlpp::Query(&conn);
  // Alter type of column to MEDIUMTEXT.
  query << "ALTER TABLE `weatherdata` CHANGE COLUMN `json` `json` mediumtext COMMENT 'raw JSON data';";
  std::clog << "Info: Changing column data type, this may take a while." << std::endl;
  if (!query.exec())
  {
    std::cerr << "Error: Could not update type of column `json`!" << std::endl;
    return -1;
  }
  std::clog << "Info: Column data type was successfully updated." << std::endl;
  // Do an ANALYZE TABLE to update it.
  query = mysqlpp::Query(&conn);
  query << "ANALYZE TABLE `weatherdata`;";
  if (!query.exec())
  {
    std::cerr << "Error: Key distribution analysis of table weatherdata failed!"
              << std::endl;
    return -1;
  }
  // All queries were successful.
  return 1;
}

bool Update065_066::updateData(const ConnectionInformation& ci)
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
  mysqlpp::Query update(&conn);
  update << "UPDATE weatherdata SET json=NULL WHERE LENGTH(json) = 65535;";
  if (!update.exec())
  {
    std::cerr << "Error: Could not reset invalid JSON data to NULL!" << std::endl;
    return false;
  }
  const auto affected = update.affected_rows();
  std::clog << "Info: Updated " << affected << ((affected==1) ? " row" : " rows")
            << " that contained invalid / truncated JSON data." << std::endl;
  // Done.
  return true;
}

} // namespace
