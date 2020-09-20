/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020  Dirk Stolle

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

#include "ForecastDataUpdate_0.8.3.hpp"
#include <iostream>
#include <memory>
#include "../api/API.hpp"
#include "../api/Apixu.hpp"
#include "../api/DarkSky.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../db/mariadb/Connection.hpp"

namespace wic
{

ForecastDataUpdate_083::ForecastDataUpdate_083(const ConnectionInformation& _ci)
: m_failed(false), ci(_ci)
{
}

void ForecastDataUpdate_083::operator()(const unsigned int startIdx, const unsigned int endIdx, const std::map<int, ApiType>& id_to_type, const db::mariadb::Result& result)
{
  m_failed = false;
  try
  {
    db::mariadb::Connection conn(ci);

    for (unsigned long int i = startIdx; i <= endIdx; ++i)
    {
      const int apiID = result.row(i).getInt32(0); // apiId is first field
      const auto iter = id_to_type.find(apiID);
      if (iter == id_to_type.end())
      {
        // Probably a newer future API type, so skip it.
        continue;
      }
      std::unique_ptr<API> api = nullptr;
      switch (iter->second)
      {
        case ApiType::Apixu:
             api.reset(new wic::Apixu());
             break;
        case ApiType::DarkSky:
             api.reset(new wic::DarkSky());
             break;
        case ApiType::OpenWeatherMap:
             api.reset(new wic::OpenWeatherMap());
             break;
        default:
             // Newer or unsupported API, go on.
             continue;
             break;
      } // switch
      Forecast fc;
      const uint_least32_t dataId = result.row(i).getInt64(1); // dataID is second field
      if (!api->parseForecast(result.row(i).column(3), fc)) // json is fourth field
      {
        std::cerr << "Error: Could not parse JSON data for data ID " << dataId
                  << "!" << std::endl;
        m_failed = true;
        return;
      }
      const auto dataTime = result.row(i).getDateTime(2); // dataTime is third field
      // Find proper weather entry.
      unsigned int idx = fc.data().size();
      for (unsigned int i = 0; i < fc.data().size(); ++i)
      {
        if (dataTime == fc.data()[i].dataTime())
        {
          idx = i;
          break;
        }
      } // for
      if (idx >= fc.data().size())
      {
        std::cerr << "Error: Could not parse find matching entry for data ID " << dataId
                  << "!" << std::endl;
        m_failed = true;
        return;
      }

      const Weather& w = fc.data()[idx];
      if (w.hasSnow())
      {
        std::string update;
        if (!w.hasRain())
        {
          update = std::string("UPDATE forecastdata SET rain=NULL, snow=")
                 + conn.quote(std::to_string(w.snow()))
                 + " WHERE dataID=" + conn.quote(std::to_string(dataId))
                 + " LIMIT 1;";
        }
        else
        {
          update = std::string("UPDATE forecastdata SET rain=")
                 + conn.quote(std::to_string(w.rain()))
                 + ", snow=" + conn.quote(std::to_string(w.snow()))
                 + " WHERE dataID=" + conn.quote(std::to_string(dataId))
                 + " LIMIT 1;";
        }
        if (conn.exec(update) < 0)
        {
          std::cerr << "Error: Could not update snow amount for data ID "
                    << dataId << "!" << std::endl;
          m_failed = true;
          return;
        }
      } // if data set has rain data
    } // for
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    m_failed = true;
    return;
  }

  m_failed = false;
  return;
}

bool ForecastDataUpdate_083::failed() const
{
  return m_failed;
}

} // namespace
