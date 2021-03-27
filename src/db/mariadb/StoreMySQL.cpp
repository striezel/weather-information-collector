/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020, 2021  Dirk Stolle

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
#include <iostream>
#include "Result.hpp"
#include "Utilities.hpp"

namespace wic
{

StoreMySQL::StoreMySQL(const ConnectionInformation& ci)
: connInfo(ci)
{
}

bool StoreMySQL::saveCurrentWeather(const ApiType type, const Location& location, const Weather& weather)
{
  // get API id
  const std::string apiName = toString(type);
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
    auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    if (!result.hasRows())
    {
      std::cerr << "Error: Could not find database record for API "
                << toString(type) << "!" << std::endl;
      return false;
    }

    const int_least32_t apiId = result.row(0).getInt32(0);
    const int_least32_t locationId = getLocationId(conn, location);
    if (locationId <= 0)
      return false;

    // Build insert query.
    std::string insertQuery = "INSERT INTO weatherdata SET apiID=" + conn.quote(std::to_string(apiId))
                + ", locationID=" + conn.quote(std::to_string(locationId));
    if (weather.hasDataTime())
    {
      insertQuery += ", dataTime=" + conn.quote(weather.dataTime());
    }
    if (weather.hasRequestTime())
    {
      insertQuery += ", requestTime=" + conn.quote(weather.requestTime());
    }
    if (weather.hasTemperatureKelvin())
    {
      insertQuery += ", temperature_K=" + conn.quote(std::to_string(weather.temperatureKelvin()));
    }
    if (weather.hasTemperatureCelsius())
    {
      insertQuery += ", temperature_C=" + conn.quote(std::to_string(weather.temperatureCelsius()));
    }
    if (weather.hasTemperatureFahrenheit())
    {
      insertQuery += ", temperature_F=" + conn.quote(std::to_string(weather.temperatureFahrenheit()));
    }
    if (weather.hasHumidity())
    {
      insertQuery += ", humidity=" + conn.quote(std::to_string(static_cast<int>(weather.humidity())));
    }
    if (weather.hasRain())
    {
      insertQuery += ", rain=" + conn.quote(std::to_string(weather.rain()));
    }
    if (weather.hasSnow())
    {
      insertQuery += ", snow=" + conn.quote(std::to_string(weather.snow()));
    }
    if (weather.hasPressure())
    {
      insertQuery += ", pressure=" + conn.quote(std::to_string(weather.pressure()));
    }
    if (weather.hasWindSpeed())
    {
      insertQuery += ", wind_speed=" + conn.quote(std::to_string(weather.windSpeed()));
    }
    if (weather.hasWindDegrees())
    {
      insertQuery += ", wind_degrees=" + conn.quote(std::to_string(weather.windDegrees()));
    }
    if (weather.hasCloudiness())
    {
      insertQuery += ", cloudiness=" + conn.quote(std::to_string(static_cast<int>(weather.cloudiness())));
    }
    if (weather.hasJson())
    {
      insertQuery += ", json=" + conn.quote(weather.json());
    }

    return conn.exec(insertQuery) >= 0;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while saving weather data! " << ex.what() << std::endl;
    return false;
  }
}

bool StoreMySQL::saveForecast(const ApiType type, const Location& location, const Forecast& forecast)
{
  if (forecast.data().empty())
  {
    // No data, nothing to save here.
    std::cerr << "Error: Forecast data is empty!" << std::endl;
    return false;
  }

  const std::string apiName = toString(type);
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
    auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    if (!result.hasRows())
    {
      std::cerr << "Error: Could not find database record for API "
                << toString(type) << "!" << std::endl;
      return false;
    }

    const int_least32_t apiId = result.row(0).getInt32(0);
    const int_least32_t locationId = getLocationId(conn, location);
    if (locationId <= 0)
      return false;

    // Insert general forecast record.
    std::string insertQuery = "INSERT INTO forecast SET apiID=" + conn.quote(std::to_string(apiId))
              + ", locationID=" + conn.quote(std::to_string(locationId))
              + ", requestTime=" + conn.quote(forecast.requestTime())
              + ",json=";
    if (forecast.hasJson())
    {
      insertQuery += conn.quote(forecast.json()) + ";";
    }
    else
    {
      insertQuery += std::string("NULL;");
    }
    if (conn.exec(insertQuery) < 0)
    {
      std::cerr << "Error: INSERT of forecast data failed. "
                << conn.errorInfo() << std::endl;
      return false;
    }
    const unsigned long long int forecastId = conn.lastInsertId();

    // Insert data elements.
    std::string dataInsert = "INSERT INTO forecastdata (forecastID, dataTime, "
        + std::string("temperature_K, temperature_C, temperature_F, humidity, rain, snow, ")
        + "pressure, wind_speed, wind_degrees, cloudiness) VALUES ";
    bool hasPreviousEntries = false;
    for (const Weather& weather: forecast.data())
    {
      if (hasPreviousEntries)
        dataInsert += ", ";
      hasPreviousEntries = true;
      dataInsert += "(" + conn.quote(std::to_string(forecastId));
      // dataTime
      dataInsert += ", " + conn.quote(weather.dataTime());
      // temperatures (K, °C, °F)
      if (weather.hasTemperatureKelvin())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.temperatureKelvin()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasTemperatureCelsius())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.temperatureCelsius()));
      }
      else
      {
        dataInsert += ", NULL";
      }
      if (weather.hasTemperatureFahrenheit())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.temperatureFahrenheit()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasHumidity())
      {
        dataInsert += ", " + conn.quote(std::to_string(static_cast<int>(weather.humidity())));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasRain())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.rain()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasSnow())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.snow()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasPressure())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.pressure()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      // wind
      if (weather.hasWindSpeed())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.windSpeed()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasWindDegrees())
      {
        dataInsert += ", " + conn.quote(std::to_string(weather.windDegrees()));
      }
      else
      {
        dataInsert += std::string(", NULL");
      }
      if (weather.hasCloudiness())
      {
        dataInsert += ", " + conn.quote(std::to_string(static_cast<int>(weather.cloudiness()))) + ")";
      }
      else
      {
        dataInsert += std::string(", NULL)");
      }
    } // for (range-based)

    if (conn.exec(dataInsert) <= 0)
    {
      std::cerr << "Error: Forecast data INSERT into database failed. "
                << conn.errorInfo() << std::endl;
      return false;
    }
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while saving forecast data! " << ex.what() << std::endl;
    return false;
  }

  // All is fine.
  return true;
}

} // namespace
