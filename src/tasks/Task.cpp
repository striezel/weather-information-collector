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

#include "Task.hpp"

namespace wic
{

Task::Task(const Location& loc, const ApiType a, const DataType d, const std::chrono::seconds& _interval)
: m_loc(loc),
  m_api(a),
  m_data(d),
  m_interval(_interval)
{
}

const Location& Task::location() const
{
  return m_loc;
}

void Task::setLocation(const Location& loc)
{
  m_loc = loc;
}

ApiType Task::api() const
{
  return m_api;
}

void Task::setApi(const ApiType a)
{
  m_api = a;
}

DataType Task::data() const
{
  return m_data;
}

void Task::setData(const DataType d)
{
  m_data = d;
}

std::chrono::seconds Task::interval() const
{
  return m_interval;
}

void Task::setInterval(const std::chrono::seconds& _interval)
{
  m_interval = _interval;
  if (_interval < std::chrono::seconds::zero())
    m_interval = std::chrono::seconds::zero();
}

bool Task::complete() const
{
  return (!m_loc.empty() && (m_api != ApiType::none)
      && (m_data != DataType::none)
      && (m_interval > std::chrono::seconds::zero()));
}

} // namespace
