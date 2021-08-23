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
#include "../../../src/db/mariadb/SourceMariaDB.hpp"
#include "../../../src/db/mariadb/StoreMariaDBBatch.hpp"
#include "../../../src/db/mariadb/Utilities.hpp"
#include "../../../src/util/Environment.hpp"
#include "../CiConnection.hpp"
#include "../CiData.hpp"
#include "../InitDB.hpp"

TEST_CASE("StoreMariaDBBatch tests")
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
      const auto Staplehurst = CiData::getStaplehurst();
      const std::size_t count = 12;
      const auto weatherBatch = CiData::generate(CiData::sampleWeatherDresden(), count);

      SourceMariaDB source(connInfo);

      // Get amount of data before insertion.
      std::vector<Weather> weather;
      REQUIRE( source.getCurrentWeather(ApiType::OpenWeatherMap, Staplehurst, weather) );
      const auto beforeInsert = weather.size();
      db::mariadb::Connection conn(connInfo);
      const auto staplehurstId = getLocationId(conn, Staplehurst);
      const auto apiId = db::API::getId(conn, ApiType::OpenWeatherMap);

      {
        StoreMariaDBBatch store(connInfo, 6);
        // Insert five  data points - one less than batch size.
        // Data amount should still be the same in database.
        for (std::size_t i = 0; i < 5; ++i)
        {
          REQUIRE( store.saveCurrentWeather(apiId, staplehurstId, weatherBatch[i]) );
        }

        REQUIRE( source.getCurrentWeather(ApiType::OpenWeatherMap, Staplehurst, weather) );
        REQUIRE( weather.size() == beforeInsert );

        // Insert sixth data point - amount should change.
        REQUIRE( store.saveCurrentWeather(apiId, staplehurstId, weatherBatch[5]) );
        REQUIRE( source.getCurrentWeather(ApiType::OpenWeatherMap, Staplehurst, weather) );
        REQUIRE( weather.size() == beforeInsert + 6 );

        // Insert some more, but stop before batch size is reached.
        for (std::size_t i = 6; i < count - 1; ++i)
        {
          REQUIRE( store.saveCurrentWeather(apiId, staplehurstId, weatherBatch[i]) );
        }
        // Amount of data in DB should not have changed.
        REQUIRE( source.getCurrentWeather(ApiType::OpenWeatherMap, Staplehurst, weather) );
        REQUIRE( weather.size() == beforeInsert + 6 );
      }

      // Weather data should still change, because StoreMariaDBBatch commits before destruction.
      REQUIRE( source.getCurrentWeather(ApiType::OpenWeatherMap, Staplehurst, weather) );
      REQUIRE( weather.size() == beforeInsert + count - 1 );
    }
  }
  else
  {
    std::clog << "Info: Test is not run within the CI environment, so it is not executed." << std::endl;
  }
}
