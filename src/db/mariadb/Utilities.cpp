/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020  Dirk Stolle

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

#include "Utilities.hpp"
#include <iostream>
#include "Result.hpp"

namespace wic
{

int_least32_t getLocationId(db::mariadb::Connection& conn, const Location& location)
{
  std::string sql = "SELECT locationID FROM location WHERE ";
  if (location.hasOwmId())
  {
    sql.append("id=" + conn.quote(std::to_string(location.owmId())));
  }
  else if (location.hasName() && location.hasCountryCode())
  {
    sql.append("name=" + conn.quote(location.name()))
       .append(" AND country_code=" + conn.quote(location.countryCode()));
  }
  else if (location.hasName())
  {
    sql.append("name=" + conn.quote(location.name()));
  }
  else if (location.hasCoordinates())
  {
    sql.append("latitude=" + conn.quote(std::to_string(location.latitude()))
        + " AND longitude=" + conn.quote(std::to_string(location.longitude())));
  }
  else if (location.hasPostcode())
  {
    sql.append("postcode=" + conn.quote(location.postcode()));
  }
  else
    return -1;
  const auto result = conn.query(sql);
  if (!result.good())
  {
    std::cerr << "Failed to get query result: " << conn.errorInfo() << "\n";
    return -1;
  }
  if (result.hasRows())
  {
    return result.row(0).getInt32(0);
  }

  // insert location, because it does not exist yet
  std::string insertQuery = "INSERT INTO location SET ";
  bool previousData = false;
  if (location.hasOwmId())
  {
    insertQuery.append("id=" + conn.quote(std::to_string(location.owmId())));
    previousData = true;
  }
  if (location.hasName())
  {
    if (previousData)
      insertQuery += ", ";
    insertQuery.append("name=" + conn.quote(location.name()));
    previousData = true;
  }
  if (location.hasCountryCode())
  {
    if (previousData)
      insertQuery += ", ";
    insertQuery.append("country_code=" + conn.quote(location.countryCode()));
    previousData = true;
  }
  if (location.hasCoordinates())
  {
    if (previousData)
      insertQuery += ", ";
    insertQuery.append("latitude=" + conn.quote(std::to_string(location.latitude())))
               .append(", longitude=" + conn.quote(std::to_string(location.longitude())));
    previousData = true;
  }
  if (location.hasPostcode())
  {
    if (previousData)
      insertQuery += ", ";
    insertQuery.append("postcode=" + conn.quote(location.postcode()));
    previousData = true;
  }
  if (!previousData)
    return -1;
  if (conn.exec(insertQuery) <= 0)
    return -1;
  const auto id = conn.lastInsertId();
  if (id != 0)
    return id;
  else
    return -1;
}

Location getLocation(db::mariadb::Connection& conn, const int_least32_t locationId)
{
  const std::string sql = "SELECT * FROM location WHERE locationID = " + conn.quote(std::to_string(locationId)) + " LIMIT 1;";
  const auto result = conn.query(sql);
  if (!result.good() || !result.hasRows())
    return Location();

  Location loc;
  const auto& row = result.row(0);
  const auto idxId = result.fieldIndex("id");
  if (!row.isNull(idxId))
    loc.setOwmId(row.getUInt32(idxId));
  const auto idxLat = result.fieldIndex("latitude");
  const auto idxLon = result.fieldIndex("longitude");
  if (!row.isNull(idxLat) && !row.isNull(idxLon))
    loc.setCoordinates(row.getFloat(idxLat), row.getFloat(idxLon));
  const auto idxName = result.fieldIndex("name");
  if (!row.isNull(idxName))
    loc.setName(row.column(idxName));
  const auto idxPost = result.fieldIndex("postcode");
  if (!row.isNull(idxPost))
    loc.setPostcode(row.column(idxPost));
  const auto idxCountry = result.fieldIndex("country_code");
  if (!row.isNull(idxCountry))
    loc.setCountryCode(row.column(idxCountry));

  return loc;
}

} // namespace
