/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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

#include "ForecastMeta.hpp"

namespace wic
{

ForecastMeta::ForecastMeta()
: m_requestTime(std::chrono::time_point<std::chrono::system_clock>())
{
}

bool ForecastMeta::hasRequestTime() const
{
  return (m_requestTime != std::chrono::time_point<std::chrono::system_clock>());
}

void ForecastMeta::setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt)
{
  m_requestTime = rt;
}

const std::chrono::time_point<std::chrono::system_clock>& ForecastMeta::requestTime() const
{
  return m_requestTime;
}

} // namespace
