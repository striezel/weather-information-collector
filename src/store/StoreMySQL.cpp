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

#include "StoreMySQL.hpp"
#include "../db/Utilities.hpp"

namespace wic
{

StoreMySQL::StoreMySQL(const ConnectionInformation& ci)
: connInfo(ci)
{
}

StoreMySQL::~StoreMySQL()
{
}

bool StoreMySQL::saveCurrentWeather(const ApiType type, const Location& location, const Weather& weather)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << toString(type) << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  if (result.num_rows() == 0)
  {
    std::cerr << "Error: Could not find database record for API "
              << toString(type) << "!" << std::endl;
    return false;
  }
  const int apiId = result[0]["apiID"];
  const int locationId = getLocationId(conn, location);
  if (locationId <= 0)
    return false;

  return saveCurrentWeather(conn, apiId, locationId, weather);
}

bool StoreMySQL::saveCurrentWeather(mysqlpp::Connection& conn, const int apiId, const int locationId, const Weather& weather)
{
  mysqlpp::Query insertQuery(&conn);
  insertQuery << "INSERT INTO weatherdata SET apiID=" << mysqlpp::quote << apiId
              << ", locationID=" << mysqlpp::quote << locationId;
  if (weather.hasDataTime())
  {
    mysqlpp::sql_datetime dt(std::chrono::system_clock::to_time_t(weather.dataTime()));
    insertQuery << ", dataTime=" << mysqlpp::quote << dt;
  }
  if (weather.hasRequestTime())
  {
    mysqlpp::sql_datetime rt(std::chrono::system_clock::to_time_t(weather.requestTime()));
    insertQuery << ", requestTime=" << mysqlpp::quote << rt;
  }
  if (weather.hasTemperatureKelvin())
  {
    insertQuery << ", temperature_K=" << mysqlpp::quote << weather.temperatureKelvin();
  }
  if (weather.hasTemperatureCelsius())
  {
    insertQuery << ", temperature_C=" << mysqlpp::quote << weather.temperatureCelsius();
  }
  if (weather.hasTemperatureFahrenheit())
  {
    insertQuery << ", temperature_F=" << mysqlpp::quote << weather.temperatureFahrenheit();
  }
  if (weather.hasHumidity())
  {
    insertQuery << ", humidity=" << mysqlpp::quote << static_cast<int>(weather.humidity());
  }
  if (weather.hasRain())
  {
    insertQuery << ", rain=" << mysqlpp::quote << weather.rain();
  }
  if (weather.hasPressure())
  {
    insertQuery << ", pressure=" << mysqlpp::quote << weather.pressure();
  }
  if (weather.hasWindSpeed())
  {
    insertQuery << ", wind_speed=" << mysqlpp::quote << weather.windSpeed();
  }
  if (weather.hasWindDegrees())
  {
    insertQuery << ", wind_degrees=" << mysqlpp::quote << weather.windDegrees();
  }
  if (weather.hasCloudiness())
  {
    insertQuery << ", cloudiness=" << mysqlpp::quote << static_cast<int>(weather.cloudiness());
  }
  if (weather.hasJson())
  {
    insertQuery << ", json=" << mysqlpp::quote << weather.json();
  }

  return insertQuery.exec();
}

bool StoreMySQL::saveForecast(const ApiType type, const Location& location, const Forecast& forecast)
{
  if (forecast.data().empty())
  {
    // No data, nothing to save here.
    std::cerr << "Error: Forecast data is empty!" << std::endl;
    return false;
  }

  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << toString(type) << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  if (result.num_rows() == 0)
  {
    std::cerr << "Error: Could not find database record for API "
              << toString(type) << "!" << std::endl;
    return false;
  }
  const int apiId = result[0]["apiID"];
  const int locationId = getLocationId(conn, location);
  if (locationId <= 0)
    return false;
  // Insert general forecast record.
  mysqlpp::Query insertQuery(&conn);
  const mysqlpp::sql_datetime reqT(std::chrono::system_clock::to_time_t(forecast.requestTime()));
  insertQuery << "INSERT INTO forecast SET apiID=" << mysqlpp::quote << apiId
              << ", locationID=" << mysqlpp::quote << locationId
              << ", requestTime=" << mysqlpp::quote << reqT
              << ",json=";
  if (forecast.hasJson())
  {
    insertQuery << mysqlpp::quote << forecast.json() << ";";
  }
  else
  {
    insertQuery << "NULL;";
  }
  if (!insertQuery.exec())
  {
    std::cerr << "Error: INSERT of forecast data failed. "
              << insertQuery.error() << std::endl;
    return false;
  }
  const unsigned int forecastId = insertQuery.insert_id();

  // Insert data elements.
  mysqlpp::Query dataInsert(&conn);
  dataInsert << "INSERT INTO forecastdata (forecastID, dataTime, "
             << "temperature_K, temperature_C, temperature_F, humidity, rain, "
             << "pressure, wind_speed, wind_degrees, cloudiness) VALUES ";
  bool hasPreviousEntries = false;
  for (const Weather& weather: forecast.data())
  {
    if (hasPreviousEntries)
      dataInsert << ", ";
    hasPreviousEntries = true;
    dataInsert << "(" << mysqlpp::quote << forecastId;
    // dataTime
    {
      mysqlpp::sql_datetime dt(std::chrono::system_clock::to_time_t(weather.dataTime()));
      dataInsert << ", " << mysqlpp::quote << dt;
    }
    // temperatures (K, °C, °F)
    if (weather.hasTemperatureKelvin())
    {
      dataInsert << ", " << mysqlpp::quote << weather.temperatureKelvin();
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasTemperatureCelsius())
    {
      dataInsert << ", " << mysqlpp::quote << weather.temperatureCelsius();
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasTemperatureFahrenheit())
    {
      dataInsert << ", " << mysqlpp::quote << weather.temperatureFahrenheit();
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasHumidity())
    {
      dataInsert << ", " << mysqlpp::quote << static_cast<int>(weather.humidity());
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasRain())
    {
      dataInsert << ", " << mysqlpp::quote << weather.rain();
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasPressure())
    {
      dataInsert << ", " << mysqlpp::quote << weather.pressure();
    }
    else
    {
      dataInsert << ", NULL";
    }
    // wind
    if (weather.hasWindSpeed())
    {
      dataInsert << ", " << mysqlpp::quote << weather.windSpeed();
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasWindDegrees())
    {
      dataInsert << ", " << mysqlpp::quote << weather.windDegrees();
    }
    else
    {
      dataInsert << ", NULL";
    }
    if (weather.hasCloudiness())
    {
      dataInsert << ", " << mysqlpp::quote << static_cast<int>(weather.cloudiness()) << ")";
    }
    else
    {
      dataInsert << ", NULL)";
    }
  } // for (range-based)

  if (!dataInsert.exec())
  {
    std::cerr << "Error: Forecast data INSERT into database failed. "
              << dataInsert.error() << std::endl;
    return false;
  }
  // All is fine.
  return true;
}

} // namespace
