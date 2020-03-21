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

#include "UpdateTo_0.8.5.hpp"
#include <algorithm>
#include <mysql++/mysql++.h>
#include "../api/OpenWeatherMap.hpp"
#include "../api/Types.hpp"
#include "../db/mysqlpp/API.hpp"

namespace wic
{

bool UpdateTo085::perform(const ConnectionInformation& ci)
{
  return updateData(ci);
}

bool UpdateTo085::updateData(const ConnectionInformation& ci)
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
  const int owmApiId = db::API::getId(ci, ApiType::OpenWeatherMap);
  if (owmApiId <= 0)
  {
    std::cerr << "Error: Could not get ID for OpenWeatherMap API!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT dataID, dataTime, json FROM `forecastdata` "
        << "  LEFT JOIN `forecast` ON forecast.forecastID=forecastdata.forecastID"
        << "  WHERE `apiID`=" << mysqlpp::quote << owmApiId
        << "  AND ISNULL(rain) AND NOT ISNULL(json) AND json LIKE '%\"rain\":{}%';";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const auto rows = result.num_rows();
  if (rows == 0)
  {
    // We are done here.
    return true;
  }

  OpenWeatherMap api;
  for (unsigned long int i = 0; i < rows; ++i)
  {
    Forecast f;
    if (!api.parseForecast(result[i]["json"].c_str(), f))
    {
      std::cerr << "Error: Could not parse JSON data!" << std::endl;
      return false;
    }
    const unsigned long int dataId = result[i]["dataId"];
    const auto dataTime = std::chrono::system_clock::from_time_t(mysqlpp::DateTime(result[i]["dataTime"]));
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

    mysqlpp::Query update(&conn);
    update << "UPDATE forecastdata SET rain=" << mysqlpp::quote << it->rain()
           << " WHERE dataID='" << dataId << "' LIMIT 1;";
    if (!update.exec())
    {
      std::cerr << "Error: Could not update rain amount for data ID "
                << dataId << "!" << std::endl;
      return false;
    }
    if ((i % 100 == 0) || (i == rows - 1))
    {
      std::cout << (i + 1) << " of " << rows << " data sets processed." << std::endl;
    }
  } // for

  return true;
}

} // namespace
