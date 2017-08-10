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
  m_latitude(std::numeric_limits<float>::quiet_NaN()),
  m_longitude(std::numeric_limits<float>::quiet_NaN()),
  m_name(""),
  m_postcode("")
{
}

uint32_t Location::id() const
{
  return m_id;
}

void Location::setId(const uint32_t newId)
{
  m_id = newId;
}

bool Location::hasId() const
{
  return (m_id != 0);
}

float Location::latitude() const
{
  return m_latitude;
}

float Location::longitude() const
{
  return m_longitude;
}

void Location::setLatitudeLongitude(const float lat, const float lon)
{
  m_latitude = lat;
  m_longitude = lon;
}

bool Location::hasLatitudeAndLongitude() const
{
  return (m_latitude >= -90.0f) && (m_latitude <= 90.0f)
      && (m_longitude >= -180.0f) && (m_longitude <= 180.0f);
}

const std::string& Location::name() const
{
  return m_name;
}

void Location::setName(const std::string& newName)
{
  m_name = newName;
}

bool Location::hasName() const
{
  return !m_name.empty();
}

const std::string& Location::postcode() const
{
  return m_postcode;
}

void Location::setPostcode(const std::string& newPostcode)
{
  m_postcode = newPostcode;
}

bool Location::hasPostcode() const
{
  return !m_postcode.empty();
}

} //namespace
