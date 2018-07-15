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

#include "Utilities.hpp"
#include "SSQLS_location.hpp"

namespace wic
{

int getLocationId(mysqlpp::Connection& conn, const Location& location)
{
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM location WHERE ";
  if (location.hasId())
  {
    query << "id=" << mysqlpp::quote << location.id();
  }
  else if (location.hasName())
  {
    query << "name=" << mysqlpp::quote << location.name();
  }
  else if (location.hasCoordinates())
  {
    query << "latitude=" << mysqlpp::quote << location.latitude()
          << " AND longitude=" << mysqlpp::quote << location.longitude();
  }
  else if (location.hasPostcode())
  {
    query << "postcode=" << mysqlpp::quote << location.postcode();
  }
  else
    return -1;
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return -1;
  }
  if (result.num_rows() > 0)
  {
    return result[0]["locationID"];
  }
  // insert location
  mysqlpp::Query insertQuery(&conn);
  insertQuery << "INSERT INTO location SET ";
  bool previousData = false;
  if (location.hasId())
  {
    insertQuery << "id=" << mysqlpp::quote << location.id();
    previousData = true;
  }
  if (location.hasName())
  {
    if (previousData)
      insertQuery << ", ";
    insertQuery << "name=" << mysqlpp::quote << location.name();
    previousData = true;
  }
  if (location.hasCoordinates())
  {
    if (previousData)
      insertQuery << ", ";
    insertQuery << "latitude=" << mysqlpp::quote << location.latitude()
                << ", longitude=" << mysqlpp::quote << location.longitude();
    previousData = true;
  }
  if (location.hasPostcode())
  {
    if (previousData)
      insertQuery << ", ";
    insertQuery << "postcode=" << mysqlpp::quote << location.postcode();
    previousData = true;
  }
  if (!previousData)
    return -1;
  if (!insertQuery.exec())
    return -1;
  return insertQuery.insert_id();
}

Location getLocation(mysqlpp::Connection& conn, const int locationId)
{
  mysqlpp::Query selectQuery(&conn);
  selectQuery << "SELECT * FROM location WHERE locationID = " << mysqlpp::quote << locationId << " LIMIT 1;";
  std::vector<location> queryData;
  selectQuery.storein(queryData);
  if (queryData.empty())
    return Location();

  Location loc;
  if (!queryData[0].id.is_null)
    loc.setId(queryData[0].id.data);
  if (!queryData[0].latitude.is_null && !queryData[0].longitude.is_null)
    loc.setCoordinates(queryData[0].latitude.data, queryData[0].longitude.data);
  if (!queryData[0].name.is_null)
    loc.setName(queryData[0].name.data);
  if (!queryData[0].postcode.is_null)
    loc.setPostcode(queryData[0].postcode.data);

  return loc;
}

} // namespace
