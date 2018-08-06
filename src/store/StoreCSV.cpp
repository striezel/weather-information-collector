/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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

#include "StoreCSV.hpp"
#include <cctype>
#include <ctime>
#include "../util/Strings.hpp"

namespace wic
{

const char StoreCSV::defaultSeparator = ';';

StoreCSV::StoreCSV(const std::string& fileName, const char separator)
: stream(fileName, std::ios_base::out | std::ios_base::app | std::ios_base::ate),
  separator(separator)
{
  // Separator must be a printable character or tabulator.
  if (!std::isprint(this->separator) && (separator != '\t'))
    this->separator = defaultSeparator;
  // Check if we have a header yet. If not, write it to file.
  if (stream.is_open())
  {
    const auto pos = stream.tellp();
    const std::string h = header();
    if ((pos != std::ofstream::pos_type(-1)) && (pos < h.size()))
    {
      stream.write(h.c_str(), h.size());
      stream.write("\n", 1);
    }
  } // if stream is open.
}

StoreCSV::~StoreCSV()
{
  if (stream.is_open())
    stream.close();
}

bool StoreCSV::saveCurrentWeather(const ApiType type, const Location& location, const Weather& weather)
{
  if (!stream.is_open())
    return false;
  // Construct CSV entry.
  // -- API
  std::string dataLine = toString(type) + separator;
  // -- location information
  if (location.hasId())
    dataLine += intToString(location.id()) + separator;
  else
    dataLine += separator;
  if (location.hasCoordinates())
  {
    dataLine += floatToString(location.latitude()) + separator
              + floatToString(location.longitude()) + separator;
  }
  else
  {
    dataLine += std::string(2, separator);
  }
  if (location.hasName())
    dataLine += location.name() + separator;
  else
    dataLine += separator;
  if (location.hasPostcode())
    dataLine += location.postcode() + separator;
  else
    dataLine += separator;
  // -- weather data
  if (weather.hasDataTime())
  {
    const std::time_t dt_c = std::chrono::system_clock::to_time_t(weather.dataTime());
    auto timeString = std::string(std::ctime(&dt_c));
    const auto pos = timeString.find('\n');
    if (pos != std::string::npos)
      timeString.erase(pos);
    dataLine += timeString + separator;
  }
  else
    dataLine += separator;
  if (weather.hasRequestTime())
  {
    const std::time_t rt_c = std::chrono::system_clock::to_time_t(weather.requestTime());
    auto timeString = std::string(std::ctime(&rt_c));
    const auto pos = timeString.find('\n');
    if (pos != std::string::npos)
      timeString.erase(pos);
    dataLine += timeString + separator;
  }
  else
    dataLine += separator;
  if (weather.hasTemperatureKelvin())
    dataLine += floatToString(weather.temperatureKelvin()) + separator;
  else
    dataLine += separator;
  if (weather.hasTemperatureCelsius())
    dataLine += floatToString(weather.temperatureCelsius()) + separator;
  else
    dataLine += separator;
  if (weather.hasTemperatureFahrenheit())
    dataLine += floatToString(weather.temperatureFahrenheit()) + separator;
  else
    dataLine += separator;
  if (weather.hasHumidity())
    dataLine += std::to_string(weather.humidity()) + separator;
  else
    dataLine += separator;
  if (weather.hasRain())
    dataLine += floatToString(weather.rain()) + separator;
  else
    dataLine += separator;
  if (weather.hasPressure())
    dataLine += intToString(weather.pressure()) + separator;
  else
    dataLine += separator;
  if (weather.hasWindSpeed())
    dataLine += floatToString(weather.windSpeed()) + separator;
  else
    dataLine += separator;
  if (weather.hasWindDegrees())
    dataLine += std::to_string(weather.windDegrees()) + separator;
  else
    dataLine += separator;
  if (weather.hasCloudiness())
    dataLine += std::to_string(weather.cloudiness());
  // append new line character
  dataLine += "\n";
  // write to stream
  stream.write(dataLine.c_str(), dataLine.size());
  return stream.good();
}

bool StoreCSV::saveForecast(const ApiType type, const Location& location, const Forecast& forecast)
{
  // TODO: Implement it.

  // Currently not implemented, so return false always.
  return false;
}

void StoreCSV::flush()
{
  stream.flush();
}

std::string StoreCSV::header() const
{
  return std::string("API") + separator + std::string("LocationID") + separator
       + std::string("Latitude") + separator + std::string("Longitude") + separator
       + std::string("City") + separator + std::string("Postcode") + separator
       // weather data
       + std::string("DataTime") + separator + std::string("RequestTime") + separator
       + std::string("Kelvin") + separator + "°C" + separator + "°F" + separator
       + std::string("RelativeHumidity") + separator + std::string("Rain") + separator
       + std::string("Pressure") + separator + std::string("WindSpeed") + separator
       + std::string("Direction") + separator + std::string("Cloudiness");
}

} // namespace
