/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2022  Dirk Stolle

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

#include "OpenMeteoFunctions.hpp"
#include <ctime>
#include <iostream>

namespace wic
{

std::optional<std::chrono::time_point<std::chrono::system_clock> > isoStringToTime(const std::string& value)
{
  // Open-Meteo time format is something like '2020-04-04T12:34', so length is 16 chars.
  if (value.size() != 16 || value[4] != '-' || value[7] != '-' || value[13] != ':')
  {
    std::cerr << "Error: " << value << " is not a valid ISO 8601 time, it "
              << "must follow the pattern 'YYYY-MM-DDThh:mm'." << std::endl;
    return std::nullopt;
  }

  std::size_t pos;
  try
  {
    const int year = std::stoi(value, &pos);
    if (pos != 4 || year < 0)
    {
      std::cerr << "Error: " << value << " is not a valid ISO 8601 time. Maybe '"
                << value.substr(0, 4) << "' is not a valid year." << std::endl;
      return std::nullopt;
    }
    const int month = std::stoi(value.substr(5, 2), &pos);
    if (pos != 2 || month > 12 || month < 1)
    {
      std::cerr << "Error: " << value << " is not a valid ISO 8601 time. Maybe '"
                << value.substr(5, 2) << "' is not a valid month." << std::endl;
      return std::nullopt;
    }
    const int day = std::stoi(value.substr(8, 2), &pos);
    if (pos != 2 || day < 1 || day > 31)
    {
      std::cerr << "Error: " << value << " is not a valid ISO 8601 time. Maybe '"
                << value.substr(8, 2) << "' is not a valid day." << std::endl;
      return std::nullopt;
    }

    const int hour = std::stoi(value.substr(11, 2), &pos);
    if (pos != 2 || hour < 0 || hour > 23)
    {
      std::cerr << "Error: " << value << " is not a valid ISO 8601 time. Maybe '"
                << value.substr(11, 2) << "' is not a valid hour." << std::endl;
      return std::nullopt;
    }
    const int minute = std::stoi(value.substr(14, 2), &pos);
    if (pos != 2 || minute < 0 || minute > 59)
    {
      std::cerr << "Error: " << value << " is not a valid ISO 8601 time. Maybe '"
                << value.substr(14, 2) << "' is not a valid minute." << std::endl;
      return std::nullopt;
    }

    struct tm tm;
    tm.tm_sec = 0;
    tm.tm_min = minute;
    tm.tm_hour = hour;
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_isdst = -1;
    const time_t tt = mktime(&tm);
    if (tt == static_cast<time_t>(-1))
    {
      std::cerr << "Error: mktime() failed when converting " << value
                << " to time_t." << std::endl;
      return std::nullopt;
    }
    return std::chrono::system_clock::from_time_t(tt);
  }
  catch(const std::exception& ex)
  {
    std::cerr << "Error: ISO 8601 time conversion threw an exception.\n"
              << "Exception message: " << ex.what() << std::endl;
    return std::nullopt;
  }
}

} // namespace
