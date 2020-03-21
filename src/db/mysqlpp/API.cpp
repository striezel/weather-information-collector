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

#include "API.hpp"

namespace wic
{

namespace db
{

int API::getId(const ConnectionInformation& ci, const ApiType type)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return -1;
  }
  return getId(conn, type);
}

int API::getId(mysqlpp::Connection& conn, const ApiType type)
{
  // get API id
  const std::string apiName = toString(type);
  mysqlpp::Query query(&conn);
  query << "SELECT * FROM api WHERE name=" << mysqlpp::quote << apiName << " LIMIT 1";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return -1;
  }
  if (result.num_rows() == 0)
  {
    return -1;
  }
  const int apiId = result[0]["apiID"];
  return apiId;
}

} // namespace

} // namespace
