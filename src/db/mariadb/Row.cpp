/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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

#include "Row.hpp"
#include <climits>
#include <stdexcept>

namespace wic::db::mariadb
{

Row::Value::Value(const bool _isNull, const std::string& _data)
: isNull(_isNull),
  data(_data)
{
}

Row::Row()
: data(std::vector<Value>())
{
}

Row::Row(std::vector<Value>&& _data)
: data(std::move(_data))
{
}

bool Row::isNull(const std::size_t index) const
{
  return data.at(index).isNull;
}

const std::string& Row::column(const std::size_t index) const
{
  return data.at(index).data;
}

const std::string& Row::operator[](const std::size_t index) const
{
  return data[index].data;
}

int32_t Row::getInt32(const std::size_t index) const
{
  const auto& value = data.at(index).data;
  std::size_t pos;
  #if INT_MAX == 2147483647LL
  // int is 32 bit.
  const int32_t i = std::stoi(value, &pos);
  #elif LONG_MAX == 2147483647LL
  // long int is 32 bit.
  const int32_t i = std::stol(value, &pos);
  #else
    #error Could not find suitable 32-bit integer type!
  #endif // INT_MAX
  if (pos != value.size())
  {
    throw std::invalid_argument(value + " is not a valid signed 32-bit integer!");
  }
  return i;
}

int64_t Row::getInt64(const std::size_t index) const
{
  const auto& value = data.at(index).data;
  std::size_t pos;
  #if LONG_MAX == 9223372036854775807LL
  // long int is 64 bit.
  const int64_t i = std::stol(value, &pos);
  #elif LLONG_MAX == 9223372036854775807LL
  // long long int is 64 bit.
  const int64_t i = std::stoll(value, &pos);
  #else
    #error Could not find suitable 64-bit integer type!
  #endif // LONG_MAX
  if (pos != value.size())
  {
    throw std::invalid_argument(value + " is not a valid signed 64-bit integer!");
  }
  return i;
}

float Row::getFloat(const std::size_t index) const
{
  const auto& value = data.at(index).data;
  std::size_t pos;
  const float f = std::stof(value, &pos);
  if (pos != value.size())
  {
    throw std::invalid_argument(value + " is not a valid float!");
  }
  return f;
}

double Row::getDouble(const std::size_t index) const
{
  const auto& value = data.at(index).data;
  std::size_t pos;
  const double d = std::stod(value, &pos);
  if (pos != value.size())
  {
    throw std::invalid_argument(value + " is not a valid double!");
  }
  return d;
}

std::chrono::time_point<std::chrono::system_clock> Row::getDateTime(const std::size_t index) const
{
  const auto& value = data.at(index).data;

  // DATETIME is something like '2020-04-04 12:34:56', so length is 19 chars.
  if (value.size() != 19 || value[4] != '-' || value[7] != '-' || value[13] != ':' || value[16] != ':')
  {
    throw std::invalid_argument(value + " is not a valid datetime, it must follow the pattern 'YYYY-MM-DD hh:mm:ss'.");
  }

  std::size_t pos;
  const int year = std::stoi(value, &pos);
  if (pos != 4 || year < 0)
  {
    throw std::invalid_argument(value + " is not a valid datetime. Maybe '" + value.substr(0, pos) + "' is not a valid year.");
  }
  const int month = std::stoi(value.substr(5, 2), &pos);
  if (pos != 2 || month > 12 || month < 1)
  {
    throw std::invalid_argument(value + " is not a valid datetime. Maybe '" + value.substr(5, 2) + "' is not a valid month.");
  }
  const int day = std::stoi(value.substr(8, 2), &pos);
  if (pos != 2 || day < 1 || day > 31)
  {
    throw std::invalid_argument(value + " is not a valid datetime. Maybe '" + value.substr(8, 2) + "' is not a valid day.");
  }

  const int hour = std::stoi(value.substr(11, 2), &pos);
  if (pos != 2 || hour < 0 || hour > 23)
  {
    throw std::invalid_argument(value + " is not a valid datetime. Maybe '" + value.substr(11, 2) + "' is not a valid hour.");
  }
  const int minute = std::stoi(value.substr(14, 2), &pos);
  if (pos != 2 || minute < 0 || minute > 59)
  {
    throw std::invalid_argument(value + " is not a valid datetime. Maybe '" + value.substr(14, 2) + "' is not a valid minute.");
  }
  const int second = std::stoi(value.substr(17, 2), &pos);
  if (pos != 2 || second < 0 || second > 59)
  {
    throw std::invalid_argument(value + " is not a valid datetime. Maybe '" + value.substr(17, 2) + "' is not a valid second.");
  }

  struct tm tm;
  tm.tm_sec = second;
  tm.tm_min = minute;
  tm.tm_hour = hour;
  tm.tm_mday = day;
  tm.tm_mon = month - 1;
  tm.tm_year = year - 1900;
  tm.tm_isdst = -1;
  const time_t tt = mktime(&tm);
  if (tt == static_cast<time_t>(-1))
  {
    throw std::invalid_argument("mktime() failed when converting " + value + " to time_t.");
  }
  return std::chrono::system_clock::from_time_t(tt);
}

} // namespace
