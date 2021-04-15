/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020, 2021  Dirk Stolle

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

#include "UpdateTo_0.6.6.hpp"
#include <iostream>
#include "../db/mariadb/Result.hpp"
#include "../db/mariadb/Structure.hpp"

namespace wic
{

bool UpdateTo066::perform(const ConnectionInformation& ci) noexcept
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

int UpdateTo066::updateStructure(const ConnectionInformation& ci) noexcept
{
  if (!Structure::tableExists(ci, "weatherdata"))
  {
    std::cerr << "Error: Table weatherdata does not exist in database "
              << ci.db() << "! Update cannot be performed." << std::endl;
    return -1;
  }
  try
  {
    db::mariadb::Connection conn(ci);

    // Get type of column `json` in table `weatherdata`.
    const std::string query = "SELECT LOWER(DATA_TYPE) AS jdt FROM information_schema.COLUMNS "
          + std::string("WHERE TABLE_NAME='weatherdata' AND COLUMN_NAME='json' ")
          + "  AND TABLE_SCHEMA=" + conn.quote(ci.db()) + " LIMIT 1;";
    const auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Error: Could not get data type of column \"json\" in table weatherdata.\n"
                << "Internal error: " << conn.errorInfo() << std::endl;
      return -1;
    }
    // SELECT was successful, but do we have some data?
    if (!result.hasRows())
    {
      std::cerr << "Error: Result of data type query is empty!" << std::endl;
      return -1;
    }
    const std::string type = result.row(0).column(0);
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

    // Alter type of column to MEDIUMTEXT.
    std::clog << "Info: Changing column data type, this may take a while." << std::endl;
    const auto affected = conn.exec("ALTER TABLE `weatherdata` CHANGE COLUMN `json` `json` mediumtext COMMENT 'raw JSON data';");
    if (affected < 0)
    {
      std::cerr << "Error: Could not update type of column `json`!" << std::endl;
      return -1;
    }
    std::clog << "Info: Column data type was successfully updated." << std::endl;
    // Do an ANALYZE TABLE to update it.
    if (conn.exec("ANALYZE TABLE `weatherdata`;") < 0)
    {
      std::cerr << "Error: Key distribution analysis of table weatherdata failed!"
                << std::endl;
      return -1;
    }
    // All queries were successful.
    return 1;
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return -1;
  }
}

bool UpdateTo066::updateData(const ConnectionInformation& ci) noexcept
{
  try
  {
    db::mariadb::Connection conn(ci);
    const auto affected = conn.exec("UPDATE weatherdata SET json=NULL WHERE LENGTH(json) = 65535;");
    if (affected < 0)
    {
      std::cerr << "Error: Could not reset invalid JSON data to NULL!" << std::endl;
      return false;
    }
    std::clog << "Info: Updated " << affected << ((affected == 1) ? " row" : " rows")
              << " that contained invalid / truncated JSON data." << std::endl;
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
