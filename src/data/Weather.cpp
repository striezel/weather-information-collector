/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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

#include "Weather.hpp"
#include <limits>

namespace wic
{

Weather::Weather()
: m_dataTime(std::chrono::time_point<std::chrono::system_clock>()),
  m_requestTime(std::chrono::time_point<std::chrono::system_clock>()),
  m_tempK(std::numeric_limits<float>::quiet_NaN()),
  m_tempC(std::numeric_limits<float>::quiet_NaN()),
  m_tempF(std::numeric_limits<float>::quiet_NaN()),
  m_windSpeed(std::numeric_limits<float>::quiet_NaN()),
  m_windDegrees(std::numeric_limits<uint16_t>::max()),
  m_cloudiness(std::numeric_limits<uint8_t>::max()),
  m_json(std::string(""))
{
}

bool Weather::hasDataTime() const
{
  return (m_dataTime != std::chrono::time_point<std::chrono::system_clock>());
}

const std::chrono::time_point<std::chrono::system_clock>& Weather::dataTime() const
{
  return m_dataTime;
}

void Weather::setDataTime(const std::chrono::time_point<std::chrono::system_clock>& dt)
{
  m_dataTime = dt;
}

bool Weather::hasRequestTime() const
{
  return (m_requestTime != std::chrono::time_point<std::chrono::system_clock>());
}

void Weather::setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt)
{
  m_requestTime = rt;
}

const std::chrono::time_point<std::chrono::system_clock>& Weather::requestTime() const
{
  return m_requestTime;
}


bool Weather::hasTemperatureKelvin() const
{
  return (m_tempK == m_tempK);
}

float Weather::temperatureKelvin() const
{
  return m_tempK;
}

void Weather::setTemperatureKelvin(const float newTempK)
{
  m_tempK = newTempK;
}

bool Weather::hasTemperatureCelsius() const
{
  return (m_tempC == m_tempC);
}

float Weather::temperatureCelsius() const
{
  return m_tempC;
}

void Weather::setTemperatureCelsius(const float newTempC)
{
  m_tempC = newTempC;
}

bool Weather::hasTemperatureFahrenheit() const
{
  return (m_tempF == m_tempF);
}

float Weather::temperatureFahrenheit() const
{
  return m_tempF;
}

void Weather::setTemperatureFahrenheit(const float newTempF)
{
  m_tempF = newTempF;
}

bool Weather::hasWindSpeed() const
{
  return (m_windSpeed == m_windSpeed);
}

float Weather::windSpeed() const
{
  return m_windSpeed;
}

void Weather::setWindSpeed(const float newSpeed)
{
  m_windSpeed = newSpeed;
}

bool Weather::hasWindDegrees() const
{
  return (0 <= m_windDegrees) && (m_windDegrees <= 360);
}

float Weather::windDegrees() const
{
  return m_windDegrees;
}

void Weather::setWindDegrees(const float newWindDegrees)
{
  m_windDegrees = newWindDegrees;
}

bool Weather::hasCloudiness() const
{
  return m_cloudiness <= 100;
}

uint8_t Weather::cloudiness() const
{
  return m_cloudiness;
}

void Weather::setCloudiness(const float newCloudiness)
{
  m_cloudiness = newCloudiness;
}

bool Weather::hasJson() const
{
  return !m_json.empty();
}

const std::string& Weather::json() const
{
  return m_json;
}

void Weather::setJson(const std::string& newJson)
{
  m_json = newJson;
}

} //namespace
