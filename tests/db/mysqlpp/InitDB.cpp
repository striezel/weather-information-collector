/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020  Dirk Stolle

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

#include "InitDB.hpp"
#include <iostream>
#include <mysql++/mysql++.h>
#include "../../../src/db/mysqlpp/Structure.hpp"

namespace wic
{

bool InitDB::createDb(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during initDb_createDb!" << std::endl;
    return false;
  }

  mysqlpp::Query createQuery(&conn);
  createQuery << "CREATE DATABASE IF NOT EXISTS `weather_information_collector` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;";
  if (!createQuery.exec())
  {
    std::cerr << "Error: Could not create datase `weather_information_collector` during createDb!" << std::endl;
    return false;
  }
  else
    return true;
}

bool InitDB::createTableApi(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during createTableApi!" << std::endl;
    return false;
  }

  mysqlpp::Query createQuery(&conn);
  createQuery << "CREATE TABLE IF NOT EXISTS `api` ("
              << "  `apiID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
              << "  `name` varchar(255) NOT NULL COMMENT 'name of the API',"
              << "  `baseURL` varchar(255) NOT NULL COMMENT 'base URL for API calls',"
              << "  PRIMARY KEY (`apiID`)"
              << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
  if (!createQuery.exec())
  {
    std::cerr << "Error: Could not create table `api` during InitDB::createTableApi!" << std::endl;
    return false;
  }
  else
    return true;
}

bool InitDB::fillTableApi(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during fillTableApi!" << std::endl;
    return false;
  }

  mysqlpp::Query insertQuery(&conn);
  insertQuery << "INSERT IGNORE INTO `api` (`apiID`, `name`, `baseURL`) VALUES "
              << "(1, 'Apixu', 'https://api.apixu.com/v1/'),"
              << "(2, 'OpenWeatherMap', 'https://api.openweathermap.org/data/2.5/'),"
              << "(3, 'DarkSky', 'https://api.darksky.net/'),"
              << "(4, 'Weatherbit', 'https://api.weatherbit.io/v2.0/'),"
              << "(5, 'Weatherstack', 'http://api.weatherstack.com/');";
  return insertQuery.exec();
}

bool InitDB::createTableForecast(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during createTableForecast!" << std::endl;
    return false;
  }

  mysqlpp::Query createQuery(&conn);
  createQuery << "CREATE TABLE IF NOT EXISTS `forecast` ("
              << "  `forecastID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
              << "  `apiID` int(10) unsigned NOT NULL COMMENT 'ID of the API that delivered the data',"
              << "  `locationID` int(10) unsigned NOT NULL COMMENT 'ID of the corresponding location',"
              << "  `requestTime` datetime NOT NULL COMMENT 'time when the API request was performed',"
              << "  `json` mediumtext COMMENT 'raw JSON data',"
              << "  PRIMARY KEY (`forecastID`),"
              << "  KEY `apiID` (`apiID`),"
              << "  KEY `locationID` (`locationID`),"
              << "  KEY `requestTime` (`requestTime`)"
              << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
  if (!createQuery.exec())
  {
    std::cerr << "Error: Could not create table `forecast` during InitDB::createTableForecast!" << std::endl;
    return false;
  }
  else
    return true;
}

bool InitDB::createTableForecastData(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during createTableForecastData!" << std::endl;
    return false;
  }

  mysqlpp::Query createQuery(&conn);
  createQuery << "CREATE TABLE IF NOT EXISTS `forecastdata` ("
              << "  `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
              << "  `forecastID` int(10) unsigned NOT NULL COMMENT 'ID of the forecast request in table forecast',"
              << "  `dataTime` datetime NOT NULL COMMENT 'time of the forecasted weather conditions',"
              << "  `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',"
              << "  `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',"
              << "  `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',"
              << "  `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',"
              << "  `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',"
              << "  `snow` float DEFAULT NULL COMMENT 'amount of snow in millimeters',"
              << "  `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',"
              << "  `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',"
              << "  `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',"
              << "  `cloudiness` tinyint(3) unsigned DEFAULT NULL COMMENT 'cloudiness in percent',"
              << "  PRIMARY KEY (`dataID`),"
              << "  KEY `forecastID` (`forecastID`),"
              << "  KEY `dataTime` (`dataTime`)"
              << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
  if (!createQuery.exec())
  {
    std::cerr << "Error: Could not create table `forecastdata` during InitDB::createTableForecastData!" << std::endl;
    return false;
  }
  else
    return true;
}

bool InitDB::createTableLocation(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during createTableLocation!" << std::endl;
    return false;
  }

  mysqlpp::Query createQuery(&conn);
  createQuery << "CREATE TABLE IF NOT EXISTS `location` ("
              << "  `locationID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
              << "  `id` int(10) unsigned DEFAULT NULL COMMENT 'ID of the location in the API',"
              << "  `latitude` float DEFAULT NULL COMMENT 'latitude of the location',"
              << "  `longitude` float DEFAULT NULL COMMENT 'longitude of the location',"
              << "  `name` varchar(255) DEFAULT NULL COMMENT 'name of the location / city',"
              << "  `postcode` varchar(25) DEFAULT NULL COMMENT 'postal code of the location',"
              << "  `country_code` char(2) DEFAULT NULL COMMENT 'ISO 3166 ALPHA-2 two letter country code of the location',"
              << "  PRIMARY KEY (`locationID`)"
              << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
  if (!createQuery.exec())
  {
    std::cerr << "Error: Could not create table `location` during InitDB::createTableLocation!" << std::endl;
    return false;
  }

  if (Structure::columnExists(ci, "location", "country_code"))
    return true;

  // Re-add column country_code. May have been lost during downgrade to version 0.8.6.
  mysqlpp::Query query(&conn);
  query << "ALTER TABLE `location`  ADD `country_code` CHAR(2) DEFAULT NULL COMMENT 'ISO 3166 ALPHA-2 two letter country code of the location'  AFTER `postcode`;";
  if (!query.exec())
  {
    std::cerr << "Error: Could not add column country_code to table location." << std::endl
              << "Internal error: " << query.error() << std::endl;
    return false;
  }
  else
    return true;
}

bool InitDB::createTableWeatherData(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during createTableWeatherData!" << std::endl;
    return false;
  }

  mysqlpp::Query createQuery(&conn);
  createQuery << "CREATE TABLE IF NOT EXISTS `weatherdata` ("
              << "  `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',"
              << "  `apiID` int(10) unsigned NOT NULL COMMENT 'ID of the API that delivered the data',"
              << "  `locationID` int(10) unsigned DEFAULT NULL COMMENT 'ID of the corresponding location data set',"
              << "  `dataTime` datetime NOT NULL COMMENT 'time when the data was received / measured',"
              << "  `requestTime` datetime NOT NULL COMMENT 'time when the API request was performed',"
              << "  `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',"
              << "  `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',"
              << "  `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',"
              << "  `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',"
              << "  `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',"
              << "  `snow` float DEFAULT NULL COMMENT 'amount of snow in millimeters',"
              << "  `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',"
              << "  `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',"
              << "  `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',"
              << "  `cloudiness` tinyint(3) unsigned DEFAULT NULL COMMENT 'cloudiness in percent',"
              << "  `json` mediumtext COMMENT 'raw JSON data',"
              << "  PRIMARY KEY (`dataID`),"
              << "  KEY `dataTime` (`dataTime`),"
              << "  KEY `apiID` (`apiID`),"
              << "  KEY `locationID` (`locationID`)"
              << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
  if (!createQuery.exec())
  {
    std::cerr << "Error: Could not create table `weatherdata` during InitDB::createTableWeatherData!" << std::endl;
    return false;
  }
  else
    return true;
}

bool InitDB::downgradeTo_0_8_6(const ConnectionInformation& ci)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database during downgradeTo_0_8_6!" << std::endl;
    return false;
  }

  mysqlpp::Query alterQuery(&conn);
  alterQuery << "ALTER TABLE `location`  DROP COLUMN `country_code`;";
  if (!alterQuery.exec())
  {
    std::cerr << "Error: Could not remove column country_code from table location." << std::endl
              << "Internal error: " << alterQuery.error() << std::endl;
    return false;
  }
  else
    return true;
}

} // namespace
