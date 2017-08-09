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

#include "Location.hpp"
#include <limits>

namespace wic
{

Location::Location()
: m_id(0),
  m_longitude(std::numeric_limits<float>::quiet_NaN()),
  m_latitude(std::numeric_limits<float>::quiet_NaN()),
  m_name(""),
  m_postcode("")
{
}

uint32_t Location::id() const
{
  return m_id;
}

float Location::longitude() const
{
  return m_longitude;
}

float Location::latitude() const
{
  return m_latitude;
}

const std::string& Location::name() const
{
  return m_name;
}

const std::string& Location::postcode() const
{
  return m_postcode;
}

} //namespace
