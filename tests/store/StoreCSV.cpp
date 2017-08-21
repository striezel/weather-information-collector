/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017  Dirk Stolle

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

#include <catch.hpp>
#include <fstream>
#include <cstdio>
#include "../../src/store/StoreCSV.hpp"

TEST_CASE("StoreCSV class")
{
  using namespace wic;

  SECTION("header value")
  {
    {
      StoreCSV csv("test1.csv");
      REQUIRE( csv.header() == "API;LocationID;Latitude;Longitude;City;Postcode;DataTime;RequestTime;Kelvin;°C;°F;RelativeHumidity;Pressure;WindSpeed;Direction;Cloudiness");
    }

    //remove file
    REQUIRE( std::remove("test1.csv") == 0 );
  }

  SECTION("header respects separator")
  {
    {
      StoreCSV csv("test1.csv", '\t');

      REQUIRE( csv.header() == "API\tLocationID\tLatitude\tLongitude\tCity\tPostcode\tDataTime\tRequestTime\tKelvin\t°C\t°F\tRelativeHumidity\tPressure\tWindSpeed\tDirection\tCloudiness");
    }

    //remove file
    REQUIRE( std::remove("test1.csv") == 0 );
  }

  SECTION("automatically writes header during construction")
  {
    std::string headerValue;
    {
      StoreCSV csv("test-header.csv");
      headerValue = csv.header();
    }

    std::string firstLine;
    {
      std::ifstream stream("test-header.csv", std::ios_base::in | std::ios_base::binary);
      std::getline(stream, firstLine);
      REQUIRE( stream.good() );
    } //scope of stream
    REQUIRE( firstLine == headerValue );

    //remove file
    REQUIRE( std::remove("test-header.csv") == 0 );
  }

  SECTION("data line")
  {
    wic::Location loc;
    loc.setId(12345);
    loc.setName("Coldtown");
    loc.setPostcode("ASDF5");
    loc.setCoordinates(-89.5f, 178.25f);

    Weather weather;
    weather.setTemperatureCelsius(-41.0f);
    weather.setTemperatureKelvin(232.15f);
    weather.setTemperatureFahrenheit(-41.8f);
    weather.setHumidity(65);
    weather.setPressure(981);
    weather.setWindSpeed(5.0);
    weather.setWindDegrees(240);
    weather.setCloudiness(95);

    //scope for CSV file
    {
      StoreCSV csv("test-data.csv");
      const bool success = csv.saveCurrentWeather(ApiType::OpenWeatherMap, loc, weather);
      REQUIRE( success );
    }

    std::string line;
    {
      std::ifstream stream("test-data.csv", std::ios_base::in | std::ios_base::binary);
      //read first line
      std::getline(stream, line);
      REQUIRE( stream.good() );
      //read second line
      std::getline(stream, line);
      REQUIRE( stream.good() );
      stream.close();
    } //scope of stream

    //remove file
    REQUIRE( std::remove("test-data.csv") == 0 );

    // API
    REQUIRE( line.find("OpenWeatherMap;") == 0 );
    // id
    REQUIRE( line.find(";12345;") != std::string::npos );
    // name
    REQUIRE( line.find(";Coldtown;") != std::string::npos );
    // postcode
    REQUIRE( line.find(";ASDF5;") != std::string::npos );
    // latitude + longitude
    REQUIRE( line.find(";-89.5;178.25;") != std::string::npos );
    // °C
    REQUIRE( line.find(";-41;") != std::string::npos );
    // °F
    REQUIRE( line.find(";-41.8;") != std::string::npos );
    // Kelvin
    REQUIRE( line.find(";232.15;") != std::string::npos );
    // humidity
    REQUIRE( line.find(";65;") != std::string::npos );
    // pressure
    REQUIRE( line.find(";981;") != std::string::npos );
    // wind speed
    REQUIRE( line.find(";5;") != std::string::npos );
    // wind direction
    REQUIRE( line.find(";240;") != std::string::npos );
    // cloudiness
    REQUIRE( line.find(";95") != std::string::npos );

    //complete data set
    REQUIRE( line == "OpenWeatherMap;12345;-89.5;178.25;Coldtown;ASDF5;;;232.15;-41;-41.8;65;981;5;240;95" );
  }
}
