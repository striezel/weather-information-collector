/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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
#include "../../../src/db/mariadb/API.hpp"
#include "../../../src/db/mariadb/Result.hpp"
#include "../../../src/db/mariadb/StoreMariaDB.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../CiData.hpp"
#include "../InitDB.hpp"

TEST_CASE("StoreMariaDB tests")
{
  using namespace wic;
  const bool isCI = isGitlabCi() || isGithubActions() || isTravisCi();

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
      StoreMariaDB store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto weather = CiData::sampleWeatherDresden();

      REQUIRE( store.saveCurrentWeather(ApiType::OpenWeatherMap, Dresden, weather) );

      // Weather data should also exist in DB now. Query some of it.
      db::mariadb::Connection conn(connInfo);
      const std::string query = "SELECT * FROM weatherdata ORDER BY dataID DESC LIMIT 1;";
      const auto result = conn.query(query);
      REQUIRE( result.good() );
      REQUIRE( result.rowCount() == 1 );

      const auto elem = result.row(0);
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("temperature_C")) );
      REQUIRE( elem.getFloat(result.fieldIndex("temperature_C")) == weather.temperatureCelsius() );
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("humidity")) );
      const uint8_t humidity = elem.getInt32(result.fieldIndex("humidity"));
      REQUIRE( humidity == weather.humidity() );
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("pressure")) );
      REQUIRE( elem.getInt32(result.fieldIndex("pressure")) == weather.pressure() );
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("wind_speed")) );
      REQUIRE( elem.getFloat(result.fieldIndex("wind_speed")) == weather.windSpeed() );
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("wind_degrees")) );
      REQUIRE( elem.getFloat(result.fieldIndex("wind_degrees")) == weather.windDegrees() );
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("json")) );
      REQUIRE( elem.column(result.fieldIndex("json")) == weather.json() );
      REQUIRE( elem.getDateTime(result.fieldIndex("dataTime")) == weather.dataTime() );
      REQUIRE( elem.getDateTime(result.fieldIndex("requestTime")) == weather.requestTime() );
    }


    SECTION("storing forecast data")
    {
      StoreMariaDB store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto forecast = CiData::sampleForecastDresden();

      REQUIRE( store.saveForecast(ApiType::OpenWeatherMap, Dresden, forecast) );

      // Forecast data should also exist in DB now. Query some of it.
      db::mariadb::Connection conn(connInfo);
      const std::string query = "SELECT * FROM forecast ORDER BY forecastID DESC LIMIT 1;";
      const auto result = conn.query(query);
      REQUIRE( result.rowCount() == 1 );

      const auto elem = result.row(0);
      REQUIRE( elem.getInt32(result.fieldIndex("apiID")) > 0 );
      REQUIRE_FALSE( elem.isNull(result.fieldIndex("json")) );
      REQUIRE( elem.column(result.fieldIndex("json")) == forecast.json() );
      REQUIRE( elem.getDateTime(result.fieldIndex("requestTime")) == forecast.requestTime() );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
