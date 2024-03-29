/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021, 2022, 2024  Dirk Stolle

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

#include "Connection.hpp"
#include <climits>
#include <ctime>
#include <iostream>
#include <vector>
#include "../Exceptions.hpp"
#include "Result.hpp"

namespace wic::db::mariadb
{

Connection::Connection(const ConnectionInformation& ci)
: conn(nullptr)
{
  conn = mysql_init(nullptr);
  if (nullptr == conn)
  {
    std::cerr << "Error: Could not allocate database handle!" << std::endl;
    throw AllocationFailure();
  }
  if (mysql_real_connect(conn, ci.hostname().c_str(), ci.user().c_str(),
          ci.password().c_str(), ci.db().c_str(), ci.port(), nullptr, 0) == nullptr)
  {
    std::string message = "Error: Could not connect to database!";
    const char * error = mysql_error(conn);
    if (error != nullptr)
    {
      message.append(" ").append(std::string(error));
    }
    std::cerr << message << std::endl;
    mysql_close(conn);
    conn = nullptr;
    throw ConnectionFailure(message);
  }
}

Connection::~Connection()
{
  if (conn != nullptr)
  {
    mysql_close(conn);
  }
}

MYSQL* Connection::raw() const
{
  return conn;
}

std::string Connection::escape(const std::string& str) const
{
  const auto len = str.size();
  const std::string::size_type bufferSize = 2 * len + 1;
  std::vector<char> buffer(bufferSize, '\0');
  // Connection `conn` cannot be nullptr, because constructor enforces it.
  const auto escapedLen = mysql_real_escape_string(conn, buffer.data(), str.c_str(), len);
  return std::string(buffer.data(), escapedLen);
}

std::string Connection::quote(const std::string& str) const
{
  return "'" + escape(str) + "'";
}

std::string Connection::quote(const std::chrono::time_point<std::chrono::system_clock>& dateTime)
{
  const std::time_t tt = std::chrono::system_clock::to_time_t(dateTime);
  struct tm tm;
  #if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__MINGW64__)
  // Note: localtime() is NOT thread-safe. Therefore we use localtime_r(), which
  // is thread-safe but may not be available on all platforms or compilers.
  struct tm* ptr = localtime_r(&tt, &tm);
  if (ptr == nullptr)
  {
    std::cerr << "Error: Date conversion with localtime_r() failed!" << std::endl;
    throw std::invalid_argument("Date conversion with localtime_r() failed!");
  }
  #else
  // MSVC does not have localtime_r, so we use localtime_s instead.
  const errno_t error = localtime_s(&tm, &tt);
  if (error != 0)
  {
    std::cerr << "Error: Date conversion with localtime_s() failed!" << std::endl;
    throw std::invalid_argument("Date conversion with localtime_s() failed!");
  }
  #endif
  const int realYear = tm.tm_year + 1900;
  const int realMonth = tm.tm_mon + 1;
  return std::string("'")
         // zero-padding for year
         .append((realYear < 1000) + (realYear < 100) + (realYear < 10), '0')
         // year itself
         .append(std::to_string(realYear))
         .append("-")
         // month + padding
         .append(realMonth < 10, '0').append(std::to_string(realMonth))
         .append("-")
         // day + padding
         .append(tm.tm_mday < 10, '0').append(std::to_string(tm.tm_mday))
         .append(" ")
         // hour + padding
         .append(tm.tm_hour < 10, '0').append(std::to_string(tm.tm_hour))
         .append(":")
         // minute + padding
         .append(tm.tm_min < 10, '0').append(std::to_string(tm.tm_min))
         .append(":")
         // second
         .append(tm.tm_sec < 10, '0').append(std::to_string(tm.tm_sec))
         .append("'");
}

Result Connection::query(const std::string& sql)
{
  if (sql.empty())
    return Result();

  if (mysql_real_query(conn, sql.c_str(), sql.length()) != 0)
  {
    std::cerr << "Error: SQL query failed! " << mysql_error(conn) << std::endl;
    return Result();
  }

  Result res;
  const unsigned int fieldCount = mysql_field_count(conn);
  if (fieldCount == 0)
  {
    // Query did not return any data.
    res.ok = true;
    return res;
  }
  res.fieldData.reserve(fieldCount);

  MYSQL_RES* result = mysql_store_result(conn);
  if (result == nullptr)
  {
    // Query did not return any data.
    // A nullptr could also mean query failure, but since mysql_real_query()
    // did already succeed, this case is taken care of.
    res.ok = true;
    return res;
  }
  const auto rowCount = mysql_num_rows(result);
  res.data.reserve(rowCount);

  // Fill field names.
  {
    const MYSQL_FIELD* fields = mysql_fetch_fields(result);
    for (unsigned int i = 0; i < fieldCount; ++i)
    {
      res.fieldData.emplace_back(typeFromDbEnum(fields[i].type), std::string(fields[i].name, fields[i].name_length));
    }
  }

  // Fill rows.
  while (MYSQL_ROW rowData = mysql_fetch_row(result))
  {
    const auto lengths = mysql_fetch_lengths(result);
    if (nullptr == lengths)
    {
      mysql_free_result(result);
      std::cerr << "Error: Could not get result lengths for current row!" << std::endl;
      res.ok = false;
      res.data.clear();
      return res;
    }
    std::vector<Row::Value> currentRow;
    currentRow.reserve(fieldCount);
    for (unsigned int i = 0; i < fieldCount; ++i)
    {
      currentRow.emplace_back(rowData[i] == nullptr, std::string(rowData[i], lengths[i]));
    }
    res.data.emplace_back(std::move(currentRow));
  }
  mysql_free_result(result);

  res.ok = true;
  return res;
}

long long int Connection::exec(const std::string& sql)
{
  if (mysql_real_query(conn, sql.c_str(), sql.length()) != 0)
  {
    std::cerr << "Error: SQL query failed! " << mysql_error(conn) << std::endl;
    return -1;
  }

  const auto affected = mysql_affected_rows(conn);
  // mysql_affected_rows() actually returns an unsigned type in its current
  // implementation, so it can never really be -1, despite its documentation.
  // Therefore the implementation of mysql_affected_rows() must use some type
  // juggling or casting for -1. Let's catch that by mapping everything not in
  // the positive range of long long int to -1.
  if (affected > LLONG_MAX || static_cast<long long int>(affected) < 0)
  {
    return -1;
  }
  return affected;
}

unsigned long long int Connection::lastInsertId()
{
  return mysql_insert_id(conn);
}

unsigned int Connection::errorCode()
{
  return mysql_errno(conn);
}

std::string Connection::errorInfo()
{
  const char * err = mysql_error(conn);
  if (nullptr == err)
    return std::string();
  else
    return std::string(err);
}

} // namespace
