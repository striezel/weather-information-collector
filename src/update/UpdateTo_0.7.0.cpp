/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019, 2020, 2021  Dirk Stolle

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

#include "UpdateTo_0.7.0.hpp"
#include <iostream>
#include "../api/OpenWeatherMap.hpp"
#include "../db/mariadb/API.hpp"
#include "../db/mariadb/Result.hpp"
#include "../db/mariadb/Structure.hpp"

namespace wic
{

bool UpdateTo070::perform(const ConnectionInformation& ci) noexcept
{
  if (!updateStructure(ci))
    return false;
  return updateData(ci);
}

bool UpdateTo070::updateStructure(const ConnectionInformation& ci) noexcept
{
  if (Structure::tableExists(ci, "forecast"))
  {
    std::clog << "Info: Table forecast already exists, creation is skipped." << std::endl;
  }
  else
  {
    try
    {
      // Create table forecast.
      db::mariadb::Connection conn(ci);
      // C++ raw string literals are great for SQL queries. :-)
      const std::string query = R"sql(
              CREATE TABLE IF NOT EXISTS `forecast` (
                `forecastID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',
                `apiID` int(10) unsigned NOT NULL COMMENT 'ID of the API that delivered the data',
                `locationID` int(10) unsigned NOT NULL COMMENT 'ID of the corresponding location',
                `requestTime` datetime NOT NULL COMMENT 'time when the API request was performed',
                `json` mediumtext COMMENT 'raw JSON data',
                PRIMARY KEY (`forecastID`),
                KEY `apiID` (`apiID`),
                KEY `locationID` (`locationID`),
                KEY `requestTime` (`requestTime`)
                ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
      )sql";
      if (conn.exec(query) < 0)
      {
        std::cerr << "Error: Could not create table forecast." << std::endl
                  << "Internal error: " << conn.errorInfo() << std::endl;
        return false;
      }
    }
    catch (const std::exception& ex)
    {
      // Should not happen, because previous connection attempts were successful,
      // but better be safe than sorry.
      std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
      return false;
    }
  } // else (i.e. table must be created)

  if (Structure::tableExists(ci, "forecastdata"))
  {
    std::clog << "Info: Table forecastdata already exists, creation is skipped." << std::endl;
  }
  else
  {
    try
    {
      // Create table forecastdata.
      db::mariadb::Connection conn(ci);
      const std::string query = R"sql(
              CREATE TABLE IF NOT EXISTS `forecastdata` (
                `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',
                `forecastID` int(10) unsigned NOT NULL COMMENT 'ID of the forecast request in table forecast',
                `dataTime` datetime NOT NULL COMMENT 'time of the forecasted weather conditions',
                `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',
                `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',
                `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',
                `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',
                `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',
                `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',
                `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',
                `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',
                `cloudiness` tinyint(3) unsigned NOT NULL COMMENT 'cloudiness in percent',
                PRIMARY KEY (`dataID`),
                KEY `forecastID` (`forecastID`),
                KEY `dataTime` (`dataTime`)
                ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
      )sql";
      if (conn.exec(query) < 0)
      {
        std::cerr << "Error: Could not create table forecastdata." << std::endl
                  << "Internal error: " << conn.errorInfo() << std::endl;
        return false;
      }
    }
    catch (const std::exception& ex)
    {
      // Should not happen, because previous connection attempts were successful,
      // but better be safe than sorry.
      std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
      return false;
    }
  } // else (i.e. table must be created)

  // All queries were successful. (Or there was nothing to do.)
  return true;
}

bool UpdateTo070::updateData(const ConnectionInformation& ci) noexcept
{
  try
  {
    db::mariadb::Connection conn(ci);
    // Get API id of OpenWeatherMap, since only OpenWeatherMap entries are of
    // relevance to the issue.
    const int owmApiId = db::API::getId(conn, ApiType::OpenWeatherMap);
    if (owmApiId <= 0)
    {
      std::cerr << "Error: Could not find API entry for OpenWeatherMap in database!"
                << std::endl;
      return false;
    }
    // Get all possible entries of OpenWeatherMap without wind direction but with
    // JSON data that contains '"deg":', i.e. values for wind direction.
    const std::string query = std::string("SELECT dataID, json FROM `weatherdata` ")
        + "  WHERE `apiID`=" + conn.quote(std::to_string(owmApiId))
        + "  AND ISNULL(wind_degrees) AND NOT ISNULL(json) AND json LIKE '%\"deg\":%';";
    const auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    const auto rows = result.rowCount();
    if (rows == 0)
    {
      // Nothing to do here.
      std::cout << "Info: No wind direction data update is needed." << std::endl;
      return true;
    }
    // Update fields.
    uint_least32_t updated = 0;
    const OpenWeatherMap owm;
    for (std::size_t i = 0; i < rows; ++i)
    {
      const uint32_t dataId = static_cast<uint32_t>(result.row(i).getInt64(0));
      const std::string json = result.row(i)[1];
      Weather w;
      if (!owm.parseCurrentWeather(json, w))
      {
        std::cerr << "Error: Could not parse JSON data of dataID " << dataId
                  << " to get weather information!" << std::endl;
        return false;
      }
      if (!w.hasWindDegrees())
        continue;
      const std::string updateQuery = std::string("UPDATE weatherdata ")
              + "  SET wind_degrees=" + conn.quote(std::to_string(w.windDegrees()))
              + "  WHERE dataID=" + conn.quote(std::to_string(dataId)) + " LIMIT 1;";
      if (conn.exec(updateQuery) < 0)
      {
        std::cerr << "Error: Could not set wind direction for dataID " << dataId
                  << " to " << w.windDegrees() << "°!" << std::endl
                  << "Internal error: " << conn.errorInfo() << std::endl;
        return false;
      }
      ++updated;
    } // for
    if (updated == 1)
    {
      std::cout << "Info: One data set has been updated with new wind direction."
                << std::endl;
    }
    else
    {
      std::cout << "Info: " << updated << " data sets have been updated with new wind direction."
                << std::endl;
    }
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }

  // All is done here.
  return true;
}

} // namespace
