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

#include "Structure.hpp"
#include <mysql++/mysql++.h>

namespace wic
{

bool Structure::tableExists(const ConnectionInformation& ci, const std::string& tableName)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT COUNT(*) AS num FROM information_schema.tables WHERE TABLE_SCHEMA="
        << mysqlpp::quote << ci.db() << " AND TABLE_NAME=" << mysqlpp::quote
        << tableName << " LIMIT 1;";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const int num = result[0]["num"];
  return (num > 0);
}

bool Structure::columnExists(const ConnectionInformation& ci, const std::string& tableName, const std::string& column)
{
  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "SELECT COUNT(*) AS num FROM information_schema.columns WHERE TABLE_SCHEMA="
        << mysqlpp::quote << ci.db() << " AND TABLE_NAME=" << mysqlpp::quote
        << tableName << " AND COLUMN_NAME=" << mysqlpp::quote << column << " LIMIT 1;";
  mysqlpp::StoreQueryResult result = query.store();
  if (!result)
  {
    std::cerr << "Failed to get query result: " << query.error() << "\n";
    return false;
  }
  const int num = result[0]["num"];
  return (num > 0);
}

} // namespace
