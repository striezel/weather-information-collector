/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020, 2021  Dirk Stolle

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
#include <iostream>

namespace wic::db
{

int API::getId(const ConnectionInformation& ci, const ApiType type)
{
  try
  {
    wic::db::mariadb::Connection conn(ci);
    return getId(conn, type);
  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return -1;
  }
}

int API::getId(const mariadb::Connection& conn, const ApiType type)
{
  // get API id
  const std::string apiName = toString(type);
  const std::string query = "SELECT apiID FROM api WHERE name=" + conn.quote(apiName) + " LIMIT 1";
  const int ret = mysql_real_query(conn.raw(), query.c_str(), query.length());
  if (ret != 0)
  {
    std::cerr << "Error: SQL query failed! " << mysql_error(conn.raw()) << std::endl;
    return -1;
  }
  MYSQL_RES* result = mysql_store_result(conn.raw());
  if (result == nullptr)
  {
    std::cerr << "Failed to get query result.\n";
    return -1;
  }
  MYSQL_ROW row = mysql_fetch_row(result);
  const int apiId = std::stoi(row[0]);
  mysql_free_result(result);
  result = nullptr;
  return apiId;
}

} // namespace
