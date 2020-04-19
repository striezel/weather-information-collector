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

#include "UpdateTo_0.8.6.hpp"
#include <iostream>
#include "../db/mariadb/API.hpp"
#include "../db/mariadb/Result.hpp"

namespace wic
{

bool UpdateTo086::perform(const ConnectionInformation& ci)
{
  return updateStructure(ci);
}

bool UpdateTo086::updateStructure(const ConnectionInformation& ci)
{
  try
  {
    db::mariadb::Connection conn(ci);

    for (const std::string& table : { "weatherdata", "forecastdata"})
    {
      std::string query = "SELECT IS_NULLABLE FROM information_schema.columns "
          + std::string("  WHERE TABLE_SCHEMA=") + conn.quote(ci.db())
          + "    AND TABLE_NAME='" + table + "' AND COLUMN_NAME='cloudiness';";
      const auto result = conn.query(query);
      if (!result.good())
      {
        std::cerr << "Error: Could not get column information of column \"cloudiness\" in table " << table << ".\n"
                  << "Internal error: " << conn.errorInfo() << std::endl;
        return false;
      }
      // SELECT was successful, but do we have some data?
      if (result.rowCount() == 0)
      {
        std::cerr << "Error: Result of column information query is empty!" << std::endl;
        return false;
      }
      const std::string isNullable = result.row(0).column(0);
      if (isNullable == "YES")
      {
        // Nothing to do here.
        std::cout << "Info: Table " << table << " already seems to be up to date." << std::endl;
      }
      else
      {
        // Alter column cloudiness to accept NULL values, too.
        query = "ALTER TABLE `" + table + "` CHANGE COLUMN `cloudiness` `cloudiness` tinyint(3) unsigned DEFAULT NULL COMMENT 'cloudiness in percent';";
        std::clog << "Info: Changing column cloudiness of table " << table
                  << ", this may take a while." << std::endl;
        if (conn.exec(query) < 0)
        {
          std::cerr << "Error: Could not update column `cloudiness` of table " << table << "!" << std::endl;
          return false;
        }
        std::clog << "Info: Column cloudiness of table " << table << " was successfully updated." << std::endl;
      } // else
    } // for (range-based)

    return true;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

} // namespace
