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

#include "SourceMySQL.hpp"
#include <mysql++/mysql++.h>
#include "../db/Utilities.hpp"
#include "../db/SSQLS_weatherdata.hpp"
#include "../db/SSQLS_forecast.hpp"
#include "../db/SSQLS_forecastdata.hpp"

namespace wic
{

SourceMySQL::SourceMySQL(const ConnectionInformation& ci)
: connInfo(ci)
{
}

SourceMySQL::~SourceMySQL()
{
}

bool SourceMySQL::getCurrentWeather(const ApiType type, const Location& location, std::vector<Weather>& weather)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << apiName << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  if (result.num_rows() == 0)
  {
    return false;
  }
  const int apiId = result[0]["apiID"];
  const int locationId = wic::getLocationId(conn, location);
  if (locationId <= 0)
    return false;

  mysqlpp::Query selectQuery(&conn);
  selectQuery << "SELECT DISTINCT * FROM weatherdata WHERE apiID=" << mysqlpp::quote << apiId
              << " AND locationID=" << mysqlpp::quote << locationId
              << " ORDER BY dataTime ASC, requestTime ASC;";
  std::vector<weatherdata> queryData;
  selectQuery.storein(queryData);
  weather.clear();
  weather.reserve(queryData.size());

  for(const auto elem : queryData)
  {
    Weather w;
    w.setDataTime(std::chrono::system_clock::from_time_t(elem.dataTime));
    w.setRequestTime(std::chrono::system_clock::from_time_t(elem.requestTime));
    if (!elem.temperature_K.is_null)
    {
      w.setTemperatureKelvin(elem.temperature_K.data);
    }
    if (!elem.temperature_C.is_null)
    {
      w.setTemperatureCelsius(elem.temperature_C.data);
    }
    if (!elem.temperature_F.is_null)
    {
      w.setTemperatureFahrenheit(elem.temperature_F.data);
    }
    if (!elem.humidity.is_null)
    {
      const uint8_t hum = elem.humidity.data;
      w.setHumidity(static_cast<int8_t>(hum));
    }
    if (!elem.rain.is_null)
    {
      w.setRain(elem.rain.data);
    }
    if (!elem.snow.is_null)
    {
      w.setSnow(elem.snow.data);
    }
    if (!elem.pressure.is_null)
    {
      w.setPressure(elem.pressure.data);
    }
    if (!elem.wind_speed.is_null)
    {
      w.setWindSpeed(elem.wind_speed.data);
    }
    if (!elem.wind_degrees.is_null)
    {
      w.setWindDegrees(elem.wind_degrees.data);
    }
    if (!elem.cloudiness.is_null)
    {
      const uint8_t cloudy = elem.cloudiness.data;
      w.setCloudiness(static_cast<int8_t>(cloudy));
    }
    if (!elem.json.is_null)
    {
      w.setJson(elem.json.data);
    }
    weather.push_back(w);
  } // for
  return true;
}

bool SourceMySQL::getMetaCurrentWeather(const ApiType type, const Location& location, std::vector<WeatherMeta>& weather)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << apiName << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  if (result.num_rows() == 0)
  {
    return false;
  }
  const int apiId = result[0]["apiID"];
  const int locationId = wic::getLocationId(conn, location);
  if (locationId <= 0)
    return false;

  mysqlpp::Query selectQuery(&conn);
  selectQuery << "SELECT dataTime, requestTime FROM weatherdata WHERE apiID=" << mysqlpp::quote << apiId
              << " AND locationID=" << mysqlpp::quote << locationId
              << " ORDER BY dataTime ASC, requestTime ASC;";
  mysqlpp::StoreQueryResult timeResult = selectQuery.store();
  weather.clear();
  weather.reserve(timeResult.num_rows());

  for(const auto elem : timeResult)
  {
    WeatherMeta w;
    w.setDataTime(std::chrono::system_clock::from_time_t(mysqlpp::DateTime(elem["dataTime"])));
    w.setRequestTime(std::chrono::system_clock::from_time_t(mysqlpp::DateTime(elem["requestTime"])));
    weather.push_back(w);
  } // for
  return true;
}

bool SourceMySQL::getForecasts(const ApiType type, const Location& location, std::vector<Forecast>& forecast)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << apiName << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  if (result.num_rows() == 0)
  {
    return false;
  }
  const int apiId = result[0]["apiID"];
  const int locationId = wic::getLocationId(conn, location);
  if (locationId <= 0)
    return false;

  mysqlpp::Query selectQueryForecasts(&conn);
  selectQueryForecasts << "SELECT DISTINCT * FROM forecast WHERE apiID=" << mysqlpp::quote << apiId
              << " AND locationID=" << mysqlpp::quote << locationId
              << " ORDER BY requestTime ASC;";
  std::vector<forecast_db> dbForecasts;
  selectQueryForecasts.storein(dbForecasts);
  forecast.clear();
  forecast.reserve(dbForecasts.size());

  for (const auto& elem : dbForecasts)
  {
    Forecast current;
    current.setRequestTime(std::chrono::system_clock::from_time_t(elem.requestTime));
    if (!elem.json.is_null)
    {
      current.setJson(elem.json.data);
    }
    const uint_least32_t forecastId = elem.forecastID;

    // Get weather elements in forecast data.
    mysqlpp::Query selectForecastData(&conn);
    selectForecastData << "SELECT DISTINCT * FROM forecastdata WHERE forecastID=" << mysqlpp::quote << forecastId
              << " ORDER BY dataTime ASC;";
    std::vector<forecastdata> forecastDataPoints;
    selectForecastData.storein(forecastDataPoints);
    std::vector<Weather> fcData;
    fcData.reserve(forecastDataPoints.size());
    for (const auto& dp : forecastDataPoints)
    {
      Weather w;
      w.setDataTime(std::chrono::system_clock::from_time_t(dp.dataTime));
      if (!dp.temperature_K.is_null)
      {
        w.setTemperatureKelvin(dp.temperature_K.data);
      }
      if (!dp.temperature_C.is_null)
      {
        w.setTemperatureCelsius(dp.temperature_C.data);
      }
      if (!dp.temperature_F.is_null)
      {
        w.setTemperatureFahrenheit(dp.temperature_F.data);
      }
      if (!dp.humidity.is_null)
      {
        const uint8_t hum = dp.humidity.data;
        w.setHumidity(static_cast<int8_t>(hum));
      }
      if (!dp.rain.is_null)
      {
        w.setRain(dp.rain.data);
      }
      if (!dp.snow.is_null)
      {
        w.setSnow(dp.snow.data);
      }
      if (!dp.pressure.is_null)
      {
        w.setPressure(dp.pressure.data);
      }
      if (!dp.wind_speed.is_null)
      {
        w.setWindSpeed(dp.wind_speed.data);
      }
      if (!dp.wind_degrees.is_null)
      {
        w.setWindDegrees(dp.wind_degrees.data);
      }
      if (!dp.cloudiness.is_null)
      {
        const uint8_t cloudy = dp.cloudiness.data;
        w.setCloudiness(static_cast<int8_t>(cloudy));
      }
      fcData.push_back(w);
    } // for (range-based, forecastDataPoints

    current.setData(fcData);
    forecast.push_back(current);
  } // for (range-based, dbForecasts)

  return true;
}

bool SourceMySQL::getMetaForecasts(const ApiType type, const Location& location, std::vector<ForecastMeta>& forecast)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << apiName << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  if (result.num_rows() == 0)
  {
    return false;
  }
  const int apiId = result[0]["apiID"];
  const int locationId = wic::getLocationId(conn, location);
  if (locationId <= 0)
    return false;

  mysqlpp::Query selectQueryForecasts(&conn);
  selectQueryForecasts << "SELECT requestTime FROM forecast WHERE apiID=" << mysqlpp::quote << apiId
              << " AND locationID=" << mysqlpp::quote << locationId
              << " ORDER BY requestTime ASC;";
  mysqlpp::StoreQueryResult timeResult = selectQueryForecasts.store();
  forecast.clear();
  forecast.reserve(timeResult.num_rows());

  for (const auto& elem : timeResult)
  {
    ForecastMeta current;
    current.setRequestTime(std::chrono::system_clock::from_time_t(mysqlpp::DateTime(elem["requestTime"])));
    forecast.push_back(current);
  } // for (range-based, timeResult)

  return true;
}

bool SourceMySQL::listApis(std::map<ApiType, int>& apis)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE NOT ISNULL(name) ORDER BY name ASC;";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const auto rows = result.num_rows();
  apis.clear();
  for (std::size_t i = 0; i < rows; ++i)
  {
    const std::string name = std::string(result[i]["name"].data());
    const int id = result[i]["apiID"];
    const ApiType api = toApiType(name);
    if (api == ApiType::none)
    {
      std::cerr << "The name \"" << name << "\" is not a valid API name!" << std::endl;
      return false;
    }
    apis[api] = id;
  } // for
  return true;
}

bool SourceMySQL::listWeatherLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT DISTINCT api.name AS apiName, location.locationID AS locID, location.name AS locName FROM weatherdata "
        << "LEFT JOIN api ON api.apiID = weatherdata.apiID "
        << "LEFT JOIN location ON weatherdata.locationID = location.locationID "
        << "ORDER BY location.name ASC;";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  locations.clear();
  const auto rows = result.num_rows();
  std::map<int, Location> locationLookup;

  for (std::size_t i = 0; i < rows; ++i)
  {
    const ApiType api = toApiType(result[i]["apiName"].c_str());
    const int locId = result[i]["locID"];
    const Location loc = getLocation(conn, locId);
    if (api == ApiType::none || loc.empty())
      return false;
    locations.push_back(std::pair<Location, ApiType>(loc, api));
  } // for
  return true;
}

bool SourceMySQL::listForecastLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT DISTINCT api.name AS apiName, location.locationID AS locID, location.name AS locName FROM forecast "
        << "LEFT JOIN api ON api.apiID = forecast.apiID "
        << "LEFT JOIN location ON forecast.locationID = location.locationID "
        << "ORDER BY location.name ASC;";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  locations.clear();
  const auto rows = result.num_rows();
  std::map<int, Location> locationLookup;

  for (std::size_t i = 0; i < rows; ++i)
  {
    const ApiType api = toApiType(result[i]["apiName"].c_str());
    const int locId = result[i]["locID"];
    const Location loc = getLocation(conn, locId);
    if (api == ApiType::none || loc.empty())
      return false;
    locations.push_back(std::pair<Location, ApiType>(loc, api));
  } // for
  return true;
}

int SourceMySQL::getLocationId(const Location& location)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(connInfo.db().c_str(), connInfo.hostname().c_str(),
                    connInfo.user().c_str(), connInfo.password().c_str(), connInfo.port()))
  {
    std::cerr << "Could not connect to database: " << conn.error() << "\n";
    return -1;
  }
  return wic::getLocationId(conn, location);
}

} // namespace
