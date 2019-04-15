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

#include "Location.hpp"
#include <cmath>
#include <limits>

namespace wic
{

Location::Location()
: m_owmId(0),
  m_latitude(std::numeric_limits<float>::quiet_NaN()),
  m_longitude(std::numeric_limits<float>::quiet_NaN()),
  m_name(""),
  m_countryCode(""),
  m_postcode("")
{
}

uint32_t Location::owmId() const
{
  return m_owmId;
}

void Location::setOwmId(const uint32_t newId)
{
  m_owmId = newId;
}

bool Location::hasOwmId() const
{
  return (m_owmId != 0);
}

float Location::latitude() const
{
  return m_latitude;
}

float Location::longitude() const
{
  return m_longitude;
}

void Location::setCoordinates(const float lat, const float lon)
{
  m_latitude = lat;
  m_longitude = lon;
}

bool Location::hasCoordinates() const
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

const std::string& Location::countryCode() const
{
  return m_countryCode;
}

void Location::setCountryCode(const std::string& newIso3166Code)
{
  // Limit country code to two characters before assignment happens.
  m_countryCode = newIso3166Code.substr(0, 2);
}

bool Location::hasCountryCode() const
{
  return !m_countryCode.empty();
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

bool Location::operator==(const Location& other) const
{
  return (m_owmId == other.m_owmId) && (m_name == other.m_name)
      && (m_countryCode == other.m_countryCode)
      && (m_postcode == other.m_postcode)
      && equalCoordinates(other);
}

bool Location::operator!=(const Location& other) const
{
  return !(*this == other);
}

bool Location::equalCoordinates(const Location& other) const
{
  const bool hll_this = hasCoordinates();
  const bool hll_other = other.hasCoordinates();
  // If one has long/lat, but the other not, then it is not equal.
  if (hll_this != hll_other)
    return false;
  // If both instances have no longitude and latitude, they are equal.
  if (!hll_other && !hll_this)
    return true;
  // Both instances have latitude and longitude: compare them.
  // Instead of comparing the values directly, we only check whether they are
  // within a 0.01° range of each other, because direct floating point comparison
  // will not always yield the results one might naively expect.
  // Furthermore, 0.01 ° corresponds to approx. 1.1 km, so they are usually in
  // the same city.
  const float epsilon = 0.01f;
  return (std::abs(m_latitude - other.m_latitude) < epsilon)
      && (std::abs(m_longitude - other.m_longitude) < epsilon);
}

bool Location::empty() const
{
  // Country code alone does not make a valid location, so do not check it.
  return (!hasOwmId() && !hasName()
       && !hasCoordinates() && !hasPostcode());
}

std::string Location::toString() const
{
  if (empty()) {
    return "<empty location>";
  }
  if (hasName())
  {
    if (hasCountryCode())
    {
      if (hasCoordinates())
      {
        return name() + ", " + countryCode() + " (" + coordinatesToString() + ")";
      }
      // name + country code only
      return name() + ", " + countryCode();
    }
    if (hasCoordinates())
    {
      return name() + " (" + coordinatesToString() + ")";
    }
    // name only
    return name();
  }
  if (hasCoordinates())
  {
    return coordinatesToString();
  }
  if (hasOwmId())
  {
    return "OWM ID " + std::to_string(owmId());
  }
  // Postcode?
  if (hasPostcode())
  {
    return "Postcode " + postcode();
  }
  // No data? Should not happen, because empty locations are caught earlier.
  return "unknown location";
}

std::string Location::coordinatesToString() const
{
  if (!hasCoordinates())
    return "<no coordinates present>";
  std::string coords;
  if (latitude() >= 0.0f)
    coords = std::to_string(latitude()) + "°N, ";
  else
    coords = std::to_string(-1.0f * latitude()) + "°S, ";
  if (longitude() >= 0.0f)
    return coords + std::to_string(longitude()) + "°E";
  else
    return coords + std::to_string(-1.0f * longitude()) + "°W";
}

} // namespace
