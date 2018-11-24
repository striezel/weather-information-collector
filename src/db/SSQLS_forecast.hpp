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

#ifndef WEATHER_INFORMATION_COLLECTOR_SSQLS_FORECAST_HPP
#define WEATHER_INFORMATION_COLLECTOR_SSQLS_FORECAST_HPP

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace wic
{

/* Specialized SQL structure for table forecast.
   SQL table creation statement is:

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

*/

sql_create_5(forecast_db,
    1, // "COMPCOUNT"
    0, // "SETCOUNT" - zero means no setters
    mysqlpp::sql_int_unsigned, forecastID,
    mysqlpp::sql_int_unsigned, apiID,
    mysqlpp::sql_int_unsigned_null, locationID,
    mysqlpp::sql_datetime, requestTime,
    mysqlpp::Null<mysqlpp::sql_mediumtext>, json)

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SSQLS_FORECAST_HPP
