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

#include <iostream>
#include <catch.hpp>
#include "../../../src/db/mysqlpp/SSQLS_forecast.hpp"
#include "../../../src/db/mysqlpp/SSQLS_weatherdata.hpp"
#include "../../../src/db/mysqlpp/StoreMySQL.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../CiData.hpp"
#include "InitDB.hpp"

TEST_CASE("StoreMySQL tests")
{
  using namespace wic;
  const bool isCI = isGitlabCi();

  // Only run this test, if we are within the CI environment.
  if (isCI)
  {
    const auto connInfo = getCiConn();
    REQUIRE( connInfo.isComplete() );

    REQUIRE( InitDB::createDb(connInfo) );
    REQUIRE( InitDB::createTableApi(connInfo) );
    REQUIRE( InitDB::createTableLocation(connInfo) );
    REQUIRE( InitDB::createTableWeatherData(connInfo) );
    REQUIRE( InitDB::createTableForecast(connInfo) );
    REQUIRE( InitDB::createTableForecastData(connInfo) );

    SECTION("storing weather data")
    {
      StoreMySQL store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto weather = CiData::sampleWeatherDresden();

      REQUIRE( store.saveCurrentWeather(ApiType::OpenWeatherMap, Dresden, weather) );

      // Weather data should also exist in DB now. Query some of it.
      mysqlpp::Connection conn(false);
      REQUIRE( conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                            connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()));
      mysqlpp::Query query(&conn);
      query << "SELECT * FROM weatherdata ORDER BY dataID DESC LIMIT 1;";
      std::vector<weatherdata> queryData;
      query.storein(queryData);
      REQUIRE( queryData.size() == 1 );

      const auto elem = queryData.at(0);
      REQUIRE_FALSE( elem.temperature_C.is_null );
      REQUIRE( elem.temperature_C.data == weather.temperatureCelsius() );
      REQUIRE_FALSE( elem.humidity.is_null );
      const uint8_t humidity = elem.humidity.data;
      REQUIRE( humidity == weather.humidity() );
      REQUIRE_FALSE( elem.pressure.is_null );
      REQUIRE( elem.pressure.data == weather.pressure() );
      REQUIRE_FALSE( elem.wind_speed.is_null );
      REQUIRE( elem.wind_speed.data == weather.windSpeed() );
      REQUIRE_FALSE( elem.wind_degrees.is_null );
      REQUIRE( elem.wind_degrees.data == weather.windDegrees() );
      REQUIRE_FALSE( elem.json.is_null );
      REQUIRE( elem.json.data == weather.json() );
      REQUIRE( std::chrono::system_clock::from_time_t(elem.dataTime) == weather.dataTime() );
      REQUIRE( std::chrono::system_clock::from_time_t(elem.requestTime) == weather.requestTime() );
    }


    SECTION("storing forecast data")
    {
      StoreMySQL store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto forecast = CiData::sampleForecastDresden();

      REQUIRE( store.saveForecast(ApiType::OpenWeatherMap, Dresden, forecast) );

      // Forecast data should also exist in DB now. Query some of it.
      mysqlpp::Connection conn(false);
      REQUIRE( conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                            connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()));
      mysqlpp::Query query(&conn);
      query << "SELECT * FROM forecast ORDER BY forecastID DESC LIMIT 1;";
      std::vector<forecast_db> queryData;
      query.storein(queryData);
      REQUIRE( queryData.size() == 1 );

      const auto elem = queryData.at(0);
      REQUIRE( elem.apiID > 0 );
      REQUIRE_FALSE( elem.json.is_null );
      REQUIRE( elem.json.data == forecast.json() );
      REQUIRE( std::chrono::system_clock::from_time_t(elem.requestTime) == forecast.requestTime() );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
