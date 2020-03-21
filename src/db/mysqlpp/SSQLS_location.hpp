/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_SSQLS_LOCATION_HPP
#define WEATHER_INFORMATION_COLLECTOR_SSQLS_LOCATION_HPP

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace wic
{

/* Specialized SQL structure for table location.
   SQL creation statement is:

   CREATE TABLE IF NOT EXISTS `location` (
    `locationID` int(10) unsigned NOT NULL COMMENT 'primary key',
    `id` int(10) unsigned DEFAULT NULL COMMENT 'ID of the location in the API',
    `latitude` float DEFAULT NULL COMMENT 'latitude of the location',
    `longitude` float DEFAULT NULL COMMENT 'longitude of the location',
    `name` varchar(255) DEFAULT NULL COMMENT 'name of the location / city',
    `postcode` varchar(25) DEFAULT NULL COMMENT 'postal code of the location',
    `country_code` char(2) DEFAULT NULL COMMENT 'ISO 3166 ALPHA-2 two letter country code of the location',
   ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/

sql_create_7(location,
    1, // "COMPCOUNT"
    0, // "SETCOUNT" - zero means no setters
    mysqlpp::sql_int_unsigned, locationID,
    mysqlpp::Null<mysqlpp::sql_int_unsigned>, id,
    mysqlpp::Null<mysqlpp::sql_float>, latitude,
    mysqlpp::Null<mysqlpp::sql_float>, longitude,
    mysqlpp::Null<mysqlpp::sql_varchar>, name,
    mysqlpp::Null<mysqlpp::sql_varchar>, postcode,
    mysqlpp::Null<mysqlpp::sql_char>, country_code)

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SSQLS_LOCATION_HPP
