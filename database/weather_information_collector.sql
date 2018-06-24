SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

CREATE DATABASE IF NOT EXISTS `weather_information_collector` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE `weather_information_collector`;

CREATE TABLE IF NOT EXISTS `api` (
`apiID` int(10) unsigned NOT NULL COMMENT 'primary key',
  `name` varchar(255) NOT NULL COMMENT 'name of the API',
  `baseURL` varchar(255) NOT NULL COMMENT 'base URL for API calls'
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

INSERT INTO `api` (`apiID`, `name`, `baseURL`) VALUES
(1, 'Apixu', 'https://api.apixu.com/v1/'),
(2, 'OpenWeatherMap', 'http://api.openweathermap.org/data/2.5/'),
(3, 'DarkSky', 'https://api.darksky.net/');

CREATE TABLE IF NOT EXISTS `location` (
`locationID` int(10) unsigned NOT NULL COMMENT 'primary key',
  `id` int(10) unsigned DEFAULT NULL COMMENT 'ID of the location in the API',
  `longitude` float DEFAULT NULL COMMENT 'longitude of the location',
  `latitude` float DEFAULT NULL COMMENT 'latitude of the location',
  `name` varchar(255) DEFAULT NULL COMMENT 'name of the location / city',
  `postcode` varchar(25) DEFAULT NULL COMMENT 'postal code of the location'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

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
  `json` text COMMENT 'raw JSON data'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


ALTER TABLE `api`
 ADD PRIMARY KEY (`apiID`);

ALTER TABLE `location`
 ADD PRIMARY KEY (`locationID`);

ALTER TABLE `weatherdata`
 ADD PRIMARY KEY (`dataID`), ADD KEY `dataTime` (`dataTime`), ADD KEY `apiID` (`apiID`), ADD KEY `locationID` (`locationID`);


ALTER TABLE `api`
MODIFY `apiID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',AUTO_INCREMENT=4;
ALTER TABLE `location`
MODIFY `locationID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key';
ALTER TABLE `weatherdata`
MODIFY `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key';
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
