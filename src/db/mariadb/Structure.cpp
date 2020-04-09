/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2020  Dirk Stolle

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
#include <iostream>
#include "Connection.hpp"
#include "Result.hpp"

namespace wic
{

bool Structure::tableExists(const ConnectionInformation& ci, const std::string& tableName)
{
  using namespace wic::db::mariadb;
  try
  {
    Connection conn(ci);
    const std::string sql = "SELECT COUNT(*) AS num FROM information_schema.tables WHERE TABLE_SCHEMA="
        + conn.quote(ci.db()) + " AND TABLE_NAME=" + conn.quote(tableName)
        + " LIMIT 1;";
    const auto result = conn.query(sql);
    if (!result.good() || !result.hasRows())
      return false;

    const int num = result.rows().at(0).getInt(0);
    return (num > 0);
  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return false;
  }
}

bool Structure::columnExists(const ConnectionInformation& ci, const std::string& tableName, const std::string& column)
{
  using namespace wic::db::mariadb;
  try
  {
    Connection conn(ci);
    const std::string sql = "SELECT COUNT(*) AS num FROM information_schema.columns WHERE TABLE_SCHEMA="
        + conn.quote(ci.db()) + " AND TABLE_NAME=" + conn.quote(tableName)
        + " AND COLUMN_NAME=" + conn.quote(column) + " LIMIT 1;";
    const auto result = conn.query(sql);
    if (!result.good() || !result.hasRows())
      return false;

    const int num = result.rows().at(0).getInt(0);
    return (num > 0);
  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return false;
  }
}

} // namespace
