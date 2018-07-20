/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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

#include "StoreMySQLBatch.hpp"
#include "../db/Utilities.hpp"

namespace wic
{

StoreMySQLBatch::StoreMySQLBatch(const ConnectionInformation& ci, const unsigned int batchSize)
: connInfo(ci),
  records(0),
  batchLimit(batchSize),
  conn(false),
  insertQuery(&conn)
{
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
  }
}

StoreMySQLBatch::~StoreMySQLBatch()
{
  commit();
}

bool StoreMySQLBatch::saveCurrentWeather(const int apiId, const int locationId, const Weather& weather)
{
  if (records == 0)
  {
    insertQuery = mysqlpp::Query(&conn);
    insertQuery << "INSERT INTO weatherdata (apiID, locationID, dataTime, requestTime, "
                << "temperature_K, temperature_C, temperature_F, humidity, rain, "
                << "pressure, wind_speed, wind_degrees, cloudiness, json) VALUES ";
  }
  else
  {
    insertQuery << ", ";
  }


  insertQuery << "(" << mysqlpp::quote << apiId << ", "
              << mysqlpp::quote << locationId;
  // dataTime
  {
    mysqlpp::sql_datetime dt(std::chrono::system_clock::to_time_t(weather.dataTime()));
    insertQuery << ", " << mysqlpp::quote << dt;
  }
  // requestTime
  {
    mysqlpp::sql_datetime rt(std::chrono::system_clock::to_time_t(weather.requestTime()));
    insertQuery << ", " << mysqlpp::quote << rt;
  }
  if (weather.hasTemperatureKelvin())
  {
    insertQuery << ", " << mysqlpp::quote << weather.temperatureKelvin();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasTemperatureCelsius())
  {
    insertQuery << ", " << mysqlpp::quote << weather.temperatureCelsius();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasTemperatureFahrenheit())
  {
    insertQuery << ", " << mysqlpp::quote << weather.temperatureFahrenheit();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasHumidity())
  {
    insertQuery << ", " << mysqlpp::quote << static_cast<int>(weather.humidity());
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasRain())
  {
    insertQuery << ", " << mysqlpp::quote << weather.rain();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasPressure())
  {
    insertQuery << ", " << mysqlpp::quote << weather.pressure();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasWindSpeed())
  {
    insertQuery << ", " << mysqlpp::quote << weather.windSpeed();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasWindDegrees())
  {
    insertQuery << ", " << mysqlpp::quote << weather.windDegrees();
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasCloudiness())
  {
    insertQuery << ", " << mysqlpp::quote << static_cast<int>(weather.cloudiness());
  }
  else
  {
    insertQuery << ", NULL";
  }
  if (weather.hasJson())
  {
    insertQuery << ", " << mysqlpp::quote << weather.json();
  }
  else
  {
    insertQuery << ", NULL";
  }
  insertQuery << ")";
  ++records;

  if (records >= batchLimit)
    return commit();
  else
    return true;
}

bool StoreMySQLBatch::commit()
{
  if (records == 0)
    return true;

  const bool success = insertQuery.exec();
  if (!success)
  {
    std::cerr << "Could not insert " << records << " record(s) into database!" << conn.error() << "\n";
  }
  records = 0;
  return success;
}

} // namespace
