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

#include "Forecast.hpp"

namespace wic
{

Forecast::Forecast()
: m_requestTime(std::chrono::time_point<std::chrono::system_clock>()),
  m_json(std::string("")),
  m_forecast(std::vector<Weather>())
{
}

bool Forecast::hasRequestTime() const
{
  return (m_requestTime != std::chrono::time_point<std::chrono::system_clock>());
}

void Forecast::setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt)
{
  m_requestTime = rt;
}

const std::chrono::time_point<std::chrono::system_clock>& Forecast::requestTime() const
{
  return m_requestTime;
}

bool Forecast::hasJson() const
{
  return !m_json.empty();
}

const std::string& Forecast::json() const
{
  return m_json;
}

void Forecast::setJson(const std::string& newJson)
{
  m_json = newJson;
}

const std::vector<Weather>& Forecast::data() const
{
  return m_forecast;
}

void Forecast::setData(const std::vector<Weather>& newData)
{
  m_forecast = newData;
}

#ifdef wic_weather_comparison
bool Forecast::operator==(const Forecast& other) const
{
  return (hasRequestTime() == other.hasRequestTime()) && (!hasRequestTime() || requestTime() == other.requestTime())
      // Original line for JSON comparison would be:
      // && (hasJson() == other.hasJson()) && (!hasJson() || json() == other.json())
      // ... but since hasJson() does only check for empty strings, we can compare directly.
      && json() == other.json()
      && data() == other.data();
}

bool Forecast::operator!=(const Forecast& other) const
{
  return !(*this == other);
}
#endif // wic_weather_comparison

} // namespace
