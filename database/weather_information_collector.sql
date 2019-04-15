/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

CREATE DATABASE IF NOT EXISTS `weather_information_collector` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE `weather_information_collector`;

--
-- Table structure for table `api`
--

/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE IF NOT EXISTS `api` (
  `apiID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',
  `name` varchar(255) NOT NULL COMMENT 'name of the API',
  `baseURL` varchar(255) NOT NULL COMMENT 'base URL for API calls',
  PRIMARY KEY (`apiID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

INSERT INTO `api` (`apiID`, `name`, `baseURL`) VALUES
(1, 'Apixu', 'https://api.apixu.com/v1/'),
(2, 'OpenWeatherMap', 'https://api.openweathermap.org/data/2.5/'),
(3, 'DarkSky', 'https://api.darksky.net/'),
(4, 'Weatherbit', 'https://api.weatherbit.io/v2.0/');

--
-- Table structure for table `forecast`
--

/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `forecastdata`
--

/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE IF NOT EXISTS `forecastdata` (
  `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',
  `forecastID` int(10) unsigned NOT NULL COMMENT 'ID of the forecast request in table forecast',
  `dataTime` datetime NOT NULL COMMENT 'time of the forecasted weather conditions',
  `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',
  `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',
  `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',
  `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',
  `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',
  `snow` float DEFAULT NULL COMMENT 'amount of snow in millimeters',
  `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',
  `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',
  `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',
  `cloudiness` tinyint(3) unsigned DEFAULT NULL COMMENT 'cloudiness in percent',
  PRIMARY KEY (`dataID`),
  KEY `forecastID` (`forecastID`),
  KEY `dataTime` (`dataTime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `location`
--

/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE IF NOT EXISTS `location` (
  `locationID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',
  `id` int(10) unsigned DEFAULT NULL COMMENT 'ID of the location in the API',
  `latitude` float DEFAULT NULL COMMENT 'latitude of the location',
  `longitude` float DEFAULT NULL COMMENT 'longitude of the location',
  `name` varchar(255) DEFAULT NULL COMMENT 'name of the location / city',
  `postcode` varchar(25) DEFAULT NULL COMMENT 'postal code of the location',
  `country_code` char(2) DEFAULT NULL COMMENT 'ISO 3166 ALPHA-2 two letter country code of the location',
  PRIMARY KEY (`locationID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `weatherdata`
--

/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE IF NOT EXISTS `weatherdata` (
  `dataID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'primary key',
  `apiID` int(10) unsigned NOT NULL COMMENT 'ID of the API that delivered the data',
  `locationID` int(10) unsigned DEFAULT NULL COMMENT 'ID of the corresponding location data set',
  `dataTime` datetime NOT NULL COMMENT 'time when the data was received / measured',
  `requestTime` datetime NOT NULL COMMENT 'time when the API request was performed',
  `temperature_K` float DEFAULT NULL COMMENT 'temperature in Kelvin',
  `temperature_C` float DEFAULT NULL COMMENT 'temperature in degrees Celsius',
  `temperature_F` float DEFAULT NULL COMMENT 'temperature in degrees Fahrenheit',
  `humidity` tinyint(3) unsigned DEFAULT NULL COMMENT 'relative humidity in percent',
  `rain` float DEFAULT NULL COMMENT 'amount of rain in millimeters',
  `snow` float DEFAULT NULL COMMENT 'amount of snow in millimeters',
  `pressure` smallint(5) unsigned DEFAULT NULL COMMENT 'air pressure in hPa',
  `wind_speed` float DEFAULT NULL COMMENT 'wind speed in meters per second',
  `wind_degrees` smallint(5) unsigned DEFAULT NULL COMMENT 'wind direction in degrees',
  `cloudiness` tinyint(3) unsigned DEFAULT NULL COMMENT 'cloudiness in percent',
  `json` mediumtext COMMENT 'raw JSON data',
  PRIMARY KEY (`dataID`),
  KEY `dataTime` (`dataTime`),
  KEY `apiID` (`apiID`),
  KEY `locationID` (`locationID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
