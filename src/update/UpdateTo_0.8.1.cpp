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

#include "UpdateTo_0.8.1.hpp"
#include <iostream>
#include "../db/mariadb/Connection.hpp"

namespace wic
{

bool UpdateTo081::perform(const ConnectionInformation& ci) noexcept
{
  return updateData(ci);
}

bool UpdateTo081::updateData(const ConnectionInformation& ci) noexcept
{
  try
  {
    db::mariadb::Connection conn(ci);

    for (const std::string& table : { "weatherdata", "forecastdata"})
    {
      std::cout << "Updating table " << table << "..." << std::endl;
      for (const std::string& column : { "temperature_C", "temperature_F", "temperature_K" })
      {
        // Update temperature values.
        std::cout << "  Updating column " << column << "..." << std::endl;
        const std::string query = std::string("UPDATE ") + table
              + "  SET " + column + " = ROUND(" + column + ")"
              + "  WHERE ABS(ROUND(" + column + ")-" + column + ") < 0.005;";
        const auto rows = conn.exec(query);
        if (rows < 0)
        {
          std::cerr << "Error: Could not adjust temperature data." << std::endl
                    << "Internal error: " << conn.errorInfo() << std::endl;
          return false;
        }
        std::cout << "  Info: ";
        if (rows == 0)
        {
          std::cout << "No temperature data in column " << column << " needs updates.";
        }
        else if (rows == 1)
        {
          std::cout << "The temperature of one data set was updated.";
        }
        else
        {
          std::cout << "The temperature of " << rows << " data sets was updated.";
        }
        std::cout << std::endl;
      } // for (range-based, inner, columns)
    } // for (range-based, outer, tables)
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }

  // All queries have been successful.
  return true;
}

} // namespace
