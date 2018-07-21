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

#ifndef WEATHER_INFORMATION_COLLECTOR_SSQLS_WEATHERDATA_HPP
#define WEATHER_INFORMATION_COLLECTOR_SSQLS_WEATHERDATA_HPP

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace wic
{

/* Specialized SQL structure for table weatherdata.
   SQL table creation statement is:

CREATE TABLE IF NOT EXISTS `weatherdata` (
  `dataID` int(10) unsigned NOT NULL COMMENT 'primary key',
  `apiID` int(10) unsigned NOT NULL COMMENT 'ID of the API that delivered the data',
  `locationID` int(10) unsigned DEFAULT NULL COMMENT 'ID of the corresponding location data set',
  `dataTime` datetime NOT NULL COMMENT 'time when the data was received / measured',
  `requestTime` datetime NOT NULL COMMENT 'time when the API request was performed',
  `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',
  `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',
  `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',
  `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',
  `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',
  `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',
  `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',
  `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',
  `cloudiness` tinyint(3) unsigned NOT NULL COMMENT 'cloudiness in percent',
  `json` mediumtext COMMENT 'raw JSON data'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

*/

sql_create_15(weatherdata,
    5, // "COMPCOUNT"
    0, // "SETCOUNT" - zero means no setters
    mysqlpp::sql_int_unsigned, dataID,
    mysqlpp::sql_int_unsigned, apiID,
    mysqlpp::sql_int_unsigned_null, locationID,
    mysqlpp::sql_datetime, dataTime,
    mysqlpp::sql_datetime, requestTime,
    mysqlpp::Null<mysqlpp::sql_float>, temperature_K,
    mysqlpp::Null<mysqlpp::sql_float>, temperature_C,
    mysqlpp::Null<mysqlpp::sql_float>, temperature_F,
    mysqlpp::Null<mysqlpp::sql_tinyint_unsigned>, humidity,
    mysqlpp::Null<mysqlpp::sql_float>, rain,
    mysqlpp::Null<mysqlpp::sql_smallint_unsigned>, pressure,
    mysqlpp::Null<mysqlpp::sql_float>, wind_speed,
    mysqlpp::Null<mysqlpp::sql_smallint_unsigned>, wind_degrees,
    mysqlpp::Null<mysqlpp::sql_tinyint_unsigned>, cloudiness,
    mysqlpp::Null<mysqlpp::sql_mediumtext>, json)

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SSQLS_WEATHERDATA_HPP
