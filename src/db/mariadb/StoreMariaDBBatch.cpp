/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020, 2021  Dirk Stolle

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

#include "StoreMariaDBBatch.hpp"
#include <iostream>
#include "Utilities.hpp"

namespace wic
{

StoreMariaDBBatch::StoreMariaDBBatch(const ConnectionInformation& ci, const unsigned int batchSize)
: records(0),
  batchLimit(batchSize),
  conn(ci),
  insertQuery(std::string())
{
}

StoreMariaDBBatch::~StoreMariaDBBatch()
{
  commit();
}

bool StoreMariaDBBatch::saveCurrentWeather(const int apiId, const int locationId, const Weather& weather)
{
  if (records == 0)
  {
    insertQuery = "INSERT INTO weatherdata (apiID, locationID, dataTime, requestTime, "
                + std::string("temperature_K, temperature_C, temperature_F, humidity, rain, snow, ")
                + std::string("pressure, wind_speed, wind_degrees, cloudiness, json) VALUES ");
  }
  else
  {
    insertQuery.append(std::string(", "));
  }


  insertQuery.append("(" + conn.quote(std::to_string(apiId)) + ", "
              + conn.quote(std::to_string(locationId)));
  // dataTime
  insertQuery.append(", " + conn.quote(weather.dataTime()));
  // requestTime
  insertQuery.append(", " + conn.quote(weather.requestTime()));
  if (weather.hasTemperatureKelvin())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.temperatureKelvin())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasTemperatureCelsius())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.temperatureCelsius())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasTemperatureFahrenheit())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.temperatureFahrenheit())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasHumidity())
  {
    insertQuery.append(", " + conn.quote(std::to_string(static_cast<int>(weather.humidity()))));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasRain())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.rain())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasSnow())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.snow())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasPressure())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.pressure())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasWindSpeed())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.windSpeed())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasWindDegrees())
  {
    insertQuery.append(", " + conn.quote(std::to_string(weather.windDegrees())));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasCloudiness())
  {
    insertQuery.append(", " + conn.quote(std::to_string(static_cast<int>(weather.cloudiness()))));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  if (weather.hasJson())
  {
    insertQuery.append(", " + conn.quote(weather.json()));
  }
  else
  {
    insertQuery.append(", NULL");
  }
  insertQuery.append(")");
  ++records;

  if (records >= batchLimit)
    return commit();
  else
    return true;
}

bool StoreMariaDBBatch::commit()
{
  if (records == 0)
    return true;

  const bool success = conn.exec(insertQuery) >= 0;
  if (!success)
  {
    std::cerr << "Could not insert " << records << " record(s) into database!\n"
              << conn.errorInfo() << "\n";
  }
  records = 0;
  return success;
}

} // namespace
