/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019  Dirk Stolle

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

#include "WeatherMeta.hpp"
#include <cmath>
#include <limits>

namespace wic
{

WeatherMeta::WeatherMeta()
: m_dataTime(std::chrono::time_point<std::chrono::system_clock>()),
  m_requestTime(std::chrono::time_point<std::chrono::system_clock>())
{
}

bool WeatherMeta::hasDataTime() const
{
  return (m_dataTime != std::chrono::time_point<std::chrono::system_clock>());
}

const std::chrono::time_point<std::chrono::system_clock>& WeatherMeta::dataTime() const
{
  return m_dataTime;
}

void WeatherMeta::setDataTime(const std::chrono::time_point<std::chrono::system_clock>& dt)
{
  m_dataTime = dt;
}

bool WeatherMeta::hasRequestTime() const
{
  return (m_requestTime != std::chrono::time_point<std::chrono::system_clock>());
}

void WeatherMeta::setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt)
{
  m_requestTime = rt;
}

const std::chrono::time_point<std::chrono::system_clock>& WeatherMeta::requestTime() const
{
  return m_requestTime;
}

} // namespace
