/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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
#include <mysql++/mysql++.h>
#include "../db/Structure.hpp"

namespace wic
{

bool UpdateTo070::perform(const ConnectionInformation& ci)
{
  return updateStructure(ci);
}

bool UpdateTo070::updateStructure(const ConnectionInformation& ci)
{
  if (Structure::tableExists(ci, "forecast"))
  {
    std::clog << "Info: Table forecast already exists, creation is skipped." << std::endl;
  }
  else
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
    // Create table forecast.
    mysqlpp::Query query(&conn);
    query << "CREATE TABLE IF NOT EXISTS `forecast` ("
          << "  `forecastID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
          << "  `apiID` int(10) unsigned NOT NULL COMMENT 'ID of the API that delivered the data',"
          << "  `locationID` int(10) unsigned NOT NULL COMMENT 'ID of the corresponding location',"
          << "  `requestTime` datetime NOT NULL COMMENT 'time when the API request was performed',"
          << "  `json` mediumtext COMMENT 'raw JSON data',"
          << "  PRIMARY KEY (`forecastID`),"
          << "  KEY `apiID` (`apiID`),"
          << "  KEY `locationID` (`locationID`),"
          << "  KEY `requestTime` (`requestTime`)"
          << "  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    if (!query.exec())
    {
      std::cerr << "Error: Could not create table forecast." << std::endl
                << "Internal error: " << query.error() << std::endl;
      return false;
    }
  } // else (i.e. table must be created)

  if (Structure::tableExists(ci, "forecastdata"))
  {
    std::clog << "Info: Table forecastdata already exists, creation is skipped." << std::endl;
  }
  else
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
    // Create table forecastdata.
    mysqlpp::Query query(&conn);
    query << "CREATE TABLE IF NOT EXISTS `forecastdata` ("
          << "  `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
          << "  `forecastID` int(10) unsigned NOT NULL COMMENT 'ID of the forecast request in table forecast',"
          << "  `dataTime` datetime NOT NULL COMMENT 'time of the forecasted weather conditions',"
          << "  `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',"
          << "  `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',"
          << "  `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',"
          << "  `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',"
          << "  `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',"
          << "  `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',"
          << "  `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',"
          << "  `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',"
          << "  `cloudiness` tinyint(3) unsigned NOT NULL COMMENT 'cloudiness in percent',"
          << "  PRIMARY KEY (`dataID`),"
          << "  KEY `forecastID` (`forecastID`),"
          << "  KEY `dataTime` (`dataTime`)"
          << "  ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    if (!query.exec())
    {
      std::cerr << "Error: Could not create table forecastdata." << std::endl
                << "Internal error: " << query.error() << std::endl;
      return false;
    }
  } // else (i.e. table must be created)

  // All queries were successful. (Or there was nothing to do.)
  return true;
}

} // namespace
