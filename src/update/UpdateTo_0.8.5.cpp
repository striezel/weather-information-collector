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

#include "UpdateTo_0.8.5.hpp"
#include <algorithm>
#include <iostream>
#include "../api/OpenWeatherMap.hpp"
#include "../api/Types.hpp"
#include "../db/mariadb/API.hpp"
#include "../db/mariadb/Result.hpp"

namespace wic
{

bool UpdateTo085::perform(const ConnectionInformation& ci)
{
  return updateData(ci);
}

bool UpdateTo085::updateData(const ConnectionInformation& ci)
{
  try
  {
    db::mariadb::Connection conn(ci);
    const int owmApiId = db::API::getId(ci, ApiType::OpenWeatherMap);
    if (owmApiId <= 0)
    {
      std::cerr << "Error: Could not get ID for OpenWeatherMap API!" << std::endl;
      return false;
    }

    const std::string query = "SELECT dataID, dataTime, json FROM `forecastdata` "
        + std::string("  LEFT JOIN `forecast` ON forecast.forecastID=forecastdata.forecastID")
        + "  WHERE `apiID`=" + conn.quote(std::to_string(owmApiId))
        + "  AND ISNULL(rain) AND NOT ISNULL(json) AND json LIKE '%\"rain\":{}%';";
    const auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    const auto rows = result.rowCount();
    if (rows == 0)
    {
      // We are done here.
      return true;
    }

    OpenWeatherMap api;
    for (unsigned long int i = 0; i < rows; ++i)
    {
      Forecast f;
      if (!api.parseForecast(result.row(i).column(2), f))
      {
        std::cerr << "Error: Could not parse JSON data for dataID " << result.row(i).column(0) << "!" << std::endl;
        return false;
      }
      const unsigned long int dataId = result.row(i).getUInt32(0);
      const auto dataTime = result.row(i).getDateTime(1);
      const auto it = std::find_if(f.data().cbegin(), f.data().cend(),
                                 [&] (const Weather& w) { return w.dataTime() == dataTime; });
      if (it == f.data().cend())
      {
        std::cerr << "Error: Could not parse find matching entry for data ID "
                  << dataId << "!" << std::endl;
        return false;
      }
      if (!it->hasRain())
      {
        if ((i % 200 == 0) || (i == rows - 1))
        {
          std::cout << (i + 1) << " of " << rows << " data sets processed." << std::endl;
        }
        // If it has no rain data, then there's nothing to do here.
        continue;
      }

      const std::string update = "UPDATE forecastdata SET rain="
             + conn.quote(std::to_string(it->rain()))
             + " WHERE dataID=" + conn.quote(std::to_string(dataId)) + " LIMIT 1;";
      if (conn.exec(update) < 0)
      {
        std::cerr << "Error: Could not update rain amount for data ID "
                  << dataId << "!" << std::endl << conn.errorInfo() << std::endl;
        return false;
      }
      if ((i % 100 == 0) || (i == rows - 1))
      {
        std::cout << (i + 1) << " of " << rows << " data sets processed." << std::endl;
      }
    } // for

    return true;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

} // namespace
