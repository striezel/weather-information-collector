/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020, 2021, 2022  Dirk Stolle

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

#include "UpdateTo_0.8.0.hpp"
#include <iostream>
#include "../db/mariadb/Connection.hpp"

namespace wic
{

bool UpdateTo080::perform(const ConnectionInformation& ci) noexcept
{
  return updateData(ci);
}

bool UpdateTo080::updateData(const ConnectionInformation& ci) noexcept
{
  try
  {
    db::mariadb::Connection conn(ci);
    for (const auto& table : { "weatherdata", "forecastdata"})
    {
      std::cout << "Updating temperature (°F) in table " << table << "..." << std::endl;
      const std::string query = std::string("UPDATE ") + table
            + "  SET temperature_F = (temperature_K-273.15) * 1.8 + 32"
            + "  WHERE ISNULL(temperature_F) AND NOT ISNULL(temperature_K);";
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
        std::cout << "No temperature data in table " << table << " needs updates.";
      }
      else if (rows == 1)
      {
        std::cout << "The temperature (°F) of one data set in table " << table << " was updated.";
      }
      else
      {
        std::cout << "The temperature (°F) of " << rows << " data sets in table " << table << " was updated.";
      }
      std::cout << std::endl;
    } // for (range-based, tables)
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
