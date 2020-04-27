/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019, 2020  Dirk Stolle

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
#include <iostream>
#include "Result.hpp"
#include "Utilities.hpp"

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
  const std::string apiName = toString(type);
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
    auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    if (!result.hasRows())
    {
      std::cerr << "Error: Could not find database record for API "
                << toString(type) << "!" << std::endl;
      return false;
    }

    const int32_t apiId = result.row(0).getInt32(0);
    const int32_t locationId = wic::getLocationId(conn, location);
    if (locationId <= 0)
      return false;

    const std::string selectQuery = "SELECT DISTINCT * FROM weatherdata WHERE apiID="
              + conn.quote(std::to_string(apiId)) + " AND locationID=" + conn.quote(std::to_string(locationId))
              + " ORDER BY dataTime ASC, requestTime ASC;";
    result = conn.query(selectQuery);
    if (!result.good())
    {
      std::cerr << "Error while retrieving weather data! " << conn.errorInfo() << std::endl;
      return false;
    }

    weather.clear();
    weather.reserve(result.rowCount());

    const auto idxDataTime = result.fieldIndex("dataTime");
    const auto idxRequestTime = result.fieldIndex("requestTime");
    const auto idxKelvin = result.fieldIndex("temperature_K");
    const auto idxCelsius = result.fieldIndex("temperature_C");
    const auto idxFahrenheit = result.fieldIndex("temperature_F");
    const auto idxHumidity = result.fieldIndex("humidity");
    const auto idxRain = result.fieldIndex("rain");
    const auto idxSnow = result.fieldIndex("snow");
    const auto idxPressure = result.fieldIndex("pressure");
    const auto idxWindSpeed = result.fieldIndex("wind_speed");
    const auto idxWindDegrees = result.fieldIndex("wind_degrees");
    const auto idxCloudiness = result.fieldIndex("cloudiness");
    const auto idxJson = result.fieldIndex("json");
    for(const auto elem : result.rows())
    {
      Weather w;
      w.setDataTime(elem.getDateTime(idxDataTime));
      w.setRequestTime(elem.getDateTime(idxRequestTime));
      if (!elem.isNull(idxKelvin))
      {
        w.setTemperatureKelvin(elem.getFloat(idxKelvin));
      }
      if (!elem.isNull(idxCelsius))
      {
        w.setTemperatureCelsius(elem.getFloat(idxCelsius));
      }
      if (!elem.isNull(idxFahrenheit))
      {
        w.setTemperatureFahrenheit(elem.getFloat(idxFahrenheit));
      }
      if (!elem.isNull(idxHumidity))
      {
        const uint8_t hum = elem.getInt32(idxHumidity);
        w.setHumidity(static_cast<int8_t>(hum));
      }
      if (!elem.isNull(idxRain))
      {
        w.setRain(elem.getFloat(idxRain));
      }
      if (!elem.isNull(idxSnow))
      {
        w.setSnow(elem.getFloat(idxSnow));
      }
      if (!elem.isNull(idxPressure))
      {
        w.setPressure(elem.getInt32(idxPressure));
      }
      if (!elem.isNull(idxWindSpeed))
      {
        w.setWindSpeed(elem.getFloat(idxWindSpeed));
      }
      if (!elem.isNull(idxWindDegrees))
      {
        w.setWindDegrees(elem.getInt32(idxWindDegrees));
      }
      if (!elem.isNull(idxCloudiness))
      {
        const uint8_t cloudy = elem.getInt32(idxCloudiness);
        w.setCloudiness(static_cast<int8_t>(cloudy));
      }
      if (!elem.isNull(idxJson))
      {
        w.setJson(elem.column(idxJson));
      }
      weather.push_back(w);
    } // for

  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving weather data! " << ex.what() << std::endl;
    return false;
  }

  return true;
}

#ifndef wic_no_metadata
bool SourceMySQL::getMetaCurrentWeather(const ApiType type, const Location& location, std::vector<WeatherMeta>& weather)
{
  const std::string apiName = toString(type);
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
    auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    if (!result.hasRows())
    {
      std::cerr << "Error: Could not find database record for API "
                << toString(type) << "!" << std::endl;
      return false;
    }

    const int_fast32_t apiId = result.row(0).getInt32(0);
    const int_fast32_t locationId = wic::getLocationId(conn, location);
    if (locationId <= 0)
      return false;

    const std::string selectQuery = "SELECT dataTime, requestTime FROM weatherdata WHERE apiID="
              + conn.quote(std::to_string(apiId))
              + " AND locationID=" + conn.quote(std::to_string(locationId))
              + " ORDER BY dataTime ASC, requestTime ASC;";

    result = conn.query(selectQuery);
    if (!result.good())
    {
      std::cerr << "Failed to get weather meta data: " << conn.errorInfo() << "\n";
      return false;
    }

    weather.clear();
    weather.reserve(result.rowCount());

    for(const auto elem : result.rows())
    {
      WeatherMeta w;
      w.setDataTime(elem.getDateTime(0)); // index zero is dataTime
      w.setRequestTime(elem.getDateTime(1)); // index one is requestTime
      weather.push_back(w);
    } // for
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving weather meta data! " << ex.what() << std::endl;
    return false;
  }

  return true;
}
#endif // wic_no_metadata

bool SourceMySQL::getForecasts(const ApiType type, const Location& location, std::vector<Forecast>& forecast)
{
  const std::string apiName = toString(type);
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
    auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    if (!result.hasRows())
    {
      std::cerr << "Error: Could not find database record for API "
                << toString(type) << "!" << std::endl;
      return false;
    }

    const int32_t apiId = result.row(0).getInt32(0);
    const int locationId = wic::getLocationId(conn, location);
    if (locationId <= 0)
      return false;

    const std::string selectQueryForecasts = "SELECT DISTINCT * FROM forecast WHERE apiID="
              + conn.quote(std::to_string(apiId))
              + " AND locationID=" + conn.quote(std::to_string(locationId))
              + " ORDER BY requestTime ASC;";
    const auto dbForecasts = conn.query(selectQueryForecasts);
    if (!dbForecasts.good())
    {
      std::cerr << "Failed to get query result for forecasts: " << conn.errorInfo() << "\n";
      return false;
    }
    forecast.clear();
    forecast.reserve(dbForecasts.rowCount());

    const auto idxForecastId = dbForecasts.fieldIndex("forecastID");
    const auto idxRequestTime = dbForecasts.fieldIndex("requestTime");
    const auto idxJson = dbForecasts.fieldIndex("json");
    for (const auto& elem : dbForecasts.rows())
    {
      Forecast current;
      current.setRequestTime(elem.getDateTime(idxRequestTime));
      if (!elem.isNull(idxJson))
      {
        current.setJson(elem.column(idxJson));
      }
      const uint_least32_t forecastId = elem.getInt32(idxForecastId);

      // Get weather elements in forecast data.
      const std::string selectForecastData = "SELECT DISTINCT * FROM forecastdata WHERE forecastID="
              + conn.quote(std::to_string(forecastId))
              + " ORDER BY dataTime ASC;";
      const auto forecastDataPoints = conn.query(selectForecastData);
      if (!forecastDataPoints.good())
      {
        std::cerr << "Failed to get query result for forecast data points: " << conn.errorInfo() << "\n";
        return false;
      }

      std::vector<Weather> fcData;
      fcData.reserve(forecastDataPoints.rowCount());

      const auto idxDataTime = forecastDataPoints.fieldIndex("dataTime");
      const auto idxKelvin = forecastDataPoints.fieldIndex("temperature_K");
      const auto idxCelsius = forecastDataPoints.fieldIndex("temperature_C");
      const auto idxFahrenheit = forecastDataPoints.fieldIndex("temperature_F");
      const auto idxHumidity = forecastDataPoints.fieldIndex("humidity");
      const auto idxRain = forecastDataPoints.fieldIndex("rain");
      const auto idxSnow = forecastDataPoints.fieldIndex("snow");
      const auto idxPressure = forecastDataPoints.fieldIndex("pressure");
      const auto idxWindSpeed = forecastDataPoints.fieldIndex("wind_speed");
      const auto idxWindDegrees = forecastDataPoints.fieldIndex("wind_degrees");
      const auto idxCloudiness = forecastDataPoints.fieldIndex("cloudiness");

      for (const auto& dp : forecastDataPoints.rows())
      {
        Weather w;
        w.setDataTime(dp.getDateTime(idxDataTime));
        if (!dp.isNull(idxKelvin))
        {
          w.setTemperatureKelvin(dp.getFloat(idxKelvin));
        }
        if (!dp.isNull(idxCelsius))
        {
          w.setTemperatureCelsius(dp.getFloat(idxCelsius));
        }
        if (!dp.isNull(idxFahrenheit))
        {
          w.setTemperatureFahrenheit(dp.getFloat(idxFahrenheit));
        }
        if (!dp.isNull(idxHumidity))
        {
          const uint8_t hum = dp.getInt32(idxHumidity);
          w.setHumidity(static_cast<int8_t>(hum));
        }
        if (!dp.isNull(idxRain))
        {
          w.setRain(dp.getFloat(idxRain));
        }
        if (!dp.isNull(idxSnow))
        {
          w.setSnow(dp.getFloat(idxSnow));
        }
        if (!dp.isNull(idxPressure))
        {
          w.setPressure(dp.getInt32(idxPressure));
        }
        if (!dp.isNull(idxWindSpeed))
        {
          w.setWindSpeed(dp.getFloat(idxWindSpeed));
        }
        if (!dp.isNull(idxWindDegrees))
        {
          w.setWindDegrees(dp.getInt32(idxWindDegrees));
        }
        if (!dp.isNull(idxCloudiness))
        {
          const uint8_t cloudy = dp.getInt32(idxCloudiness);
          w.setCloudiness(static_cast<int8_t>(cloudy));
        }
        fcData.push_back(w);
      } // for (range-based, forecastDataPoints

      current.setData(fcData);
      forecast.push_back(current);
    } // for (range-based, dbForecasts)
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving forecast data! " << ex.what() << std::endl;
    return false;
  }

  return true;
}

#ifndef wic_no_metadata
bool SourceMySQL::getMetaForecasts(const ApiType type, const Location& location, std::vector<ForecastMeta>& forecast)
{
  const std::string apiName = toString(type);
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
    auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    if (!result.hasRows())
    {
      std::cerr << "Error: Could not find database record for API "
                << toString(type) << "!" << std::endl;
      return false;
    }

    const uint32_t apiId = result.row(0).getInt32(0);
    const int_least32_t locationId = wic::getLocationId(conn, location);
    if (locationId <= 0)
      return false;

    const std::string selectQueryForecasts = "SELECT requestTime FROM forecast WHERE apiID="
            + conn.quote(std::to_string(apiId))
            + " AND locationID=" + conn.quote(std::to_string(locationId))
            + " ORDER BY requestTime ASC;";
    const auto timeResult = conn.query(selectQueryForecasts);
    if (!timeResult.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }

    forecast.clear();
    forecast.reserve(timeResult.rowCount());

    for (const auto& elem : timeResult.rows())
    {
      ForecastMeta current;
      current.setRequestTime(elem.getDateTime(0));
      forecast.push_back(current);
    } // for (range-based, timeResult)

    return true;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving forecast meta data! " << ex.what() << std::endl;
    return false;
  }
}
#endif

bool SourceMySQL::listApis(std::map<ApiType, int>& apis)
{
  try
  {
    db::mariadb::Connection conn(connInfo);
    const auto result = conn.query("SELECT apiID, name FROM api WHERE NOT ISNULL(name) ORDER BY name ASC;");
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }

    apis.clear();
    for (const auto& row : result.rows())
    {
      const int_least32_t id = row.getInt32(0); // index zero is apiID
      const std::string name = row.column(1); // index one is name
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
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving API listing! " << ex.what() << std::endl;
    return false;
  }
}

bool SourceMySQL::listWeatherLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations)
{
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT DISTINCT api.name AS apiName, location.locationID AS locID, location.name AS locName FROM weatherdata "
        + std::string("LEFT JOIN api ON api.apiID = weatherdata.apiID ")
        + std::string("LEFT JOIN location ON weatherdata.locationID = location.locationID ")
        + std::string("ORDER BY location.name ASC;");
    const auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    locations.clear();

    const auto idxApiName = result.fieldIndex("apiName");
    const auto idxLocationId = result.fieldIndex("locID");
    for (const auto& row: result.rows())
    {
      const ApiType api = toApiType(row.column(idxApiName));
      const int_least32_t locId = row.getInt32(idxLocationId);
      const Location loc = getLocation(conn, locId);
      if (api == ApiType::none || loc.empty())
        return false;
      locations.push_back(std::pair<Location, ApiType>(loc, api));
    } // for
    return true;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving weather location listing! " << ex.what() << std::endl;
    return false;
  }
}

bool SourceMySQL::listForecastLocationsWithApi(std::vector<std::pair<Location, ApiType> >& locations)
{
  try
  {
    db::mariadb::Connection conn(connInfo);
    const std::string query = "SELECT DISTINCT api.name AS apiName, location.locationID AS locID, location.name AS locName FROM forecast "
        + std::string("LEFT JOIN api ON api.apiID = forecast.apiID ")
        + std::string("LEFT JOIN location ON forecast.locationID = location.locationID ")
        + std::string("ORDER BY location.name ASC;");
    const auto result = conn.query(query);
    if (!result.good())
    {
      std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
      return false;
    }
    locations.clear();

    const auto idxApiName = result.fieldIndex("apiName");
    const auto idxLocationId = result.fieldIndex("locID");
    for (const auto& row : result.rows())
    {
      const ApiType api = toApiType(row.column(idxApiName));
      const int_least32_t locId = row.getInt32(idxLocationId);
      const Location loc = getLocation(conn, locId);
      if (api == ApiType::none || loc.empty())
        return false;
      locations.push_back(std::pair<Location, ApiType>(loc, api));
    } // for
    return true;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving forecast location listing! " << ex.what() << std::endl;
    return false;
  }
}

int SourceMySQL::getLocationId(const Location& location)
{
  try
  {
    db::mariadb::Connection conn(connInfo);
    return wic::getLocationId(conn, location);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error while retrieving location id! " << ex.what() << std::endl;
    return -1;
  }
}

} // namespace
