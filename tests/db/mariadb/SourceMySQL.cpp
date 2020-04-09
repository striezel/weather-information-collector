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
#include "../../../src/db/mariadb/SourceMySQL.hpp"
#include "../../../src/db/mariadb/StoreMySQL.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../CiData.hpp"
#include "../InitDB.hpp"

TEST_CASE("SourceMySQL tests")
{
  using namespace wic;
  const bool isCI = isGitlabCi() || isTravisCi();

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

    SECTION("storing and retrieving weather data")
    {
      StoreMySQL store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto weather = CiData::sampleWeatherDresden();

      REQUIRE( store.saveCurrentWeather(ApiType::OpenWeatherMap, Dresden, weather) );

      // Weather data should also exist in DB now. Query some of it.
      SourceMySQL source(connInfo);
      std::vector<Weather> dbWeather;
      // -- Retrieval should succeed.
      REQUIRE( source.getCurrentWeather(ApiType::OpenWeatherMap, Dresden, dbWeather) );
      // -- Retrieved data must not be empty.
      REQUIRE_FALSE( dbWeather.empty() );
      const Weather& latest = dbWeather.back();
      // -- Latest retrieved weather element should match saved data.
      REQUIRE( latest == weather );
    }

    SECTION("storing weather data and retrieving its meta data")
    {
      StoreMySQL store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto weather = CiData::sampleWeatherDresden();

      REQUIRE( store.saveCurrentWeather(ApiType::OpenWeatherMap, Dresden, weather) );

      // Weather data should also exist in DB now. Query some of it.
      SourceMySQL source(connInfo);
      std::vector<WeatherMeta> metaWeather;
      // -- Retrieval should succeed.
      REQUIRE( source.getMetaCurrentWeather(ApiType::OpenWeatherMap, Dresden, metaWeather) );
      // -- Retrieved data must not be empty.
      REQUIRE_FALSE( metaWeather.empty() );
      const WeatherMeta& latest = metaWeather.back();
      // -- Latest retrieved weather element should match saved data.
      REQUIRE( latest.hasDataTime() );
      REQUIRE( latest.hasRequestTime() );
      REQUIRE( latest.dataTime() == weather.dataTime() );
      REQUIRE( latest.requestTime() == weather.requestTime() );
    }


    SECTION("storing and retrieving forecast data")
    {
      StoreMySQL store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto forecast = CiData::sampleForecastDresden();

      REQUIRE( store.saveForecast(ApiType::OpenWeatherMap, Dresden, forecast) );

      // Forecast data should also exist in DB now. Query some of it.
      SourceMySQL source(connInfo);
      std::vector<Forecast> dbForecast;
      // -- Retrieval should succeed.
      REQUIRE( source.getForecasts(ApiType::OpenWeatherMap, Dresden, dbForecast) );
      // -- Retrieved data must not be empty.
      REQUIRE_FALSE( dbForecast.empty() );
      const Forecast& latest = dbForecast.back();
      REQUIRE( latest.data().size() == forecast.data().size() );
      // -- Latest retrieved forecast element should match saved data.
      REQUIRE( latest == forecast );
    }

    SECTION("storing forecast data and retrieving its meta data")
    {
      StoreMySQL store(connInfo);
      const auto Dresden = CiData::getDresden();
      const auto forecast = CiData::sampleForecastDresden();

      REQUIRE( store.saveForecast(ApiType::OpenWeatherMap, Dresden, forecast) );

      // Forecast data should also exist in DB now. Query metadata of it.
      SourceMySQL source(connInfo);
      std::vector<ForecastMeta> metaForecast;
      // -- Retrieval should succeed.
      REQUIRE( source.getMetaForecasts(ApiType::OpenWeatherMap, Dresden, metaForecast) );
      // -- Retrieved data must not be empty.
      REQUIRE_FALSE( metaForecast.empty() );
      const ForecastMeta& latest = metaForecast.back();
      REQUIRE( latest.hasRequestTime() );
      // -- Latest retrieved forecast element should match saved data.
      REQUIRE( latest.requestTime() == forecast.requestTime() );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
