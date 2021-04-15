/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2020, 2021  Dirk Stolle

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

#include "UpdateTo_0.9.7.hpp"
#include <iostream>
#include "../db/mariadb/API.hpp"

namespace wic
{

bool UpdateTo097::perform(const ConnectionInformation& ci) noexcept
{
  return updateData(ci);
}

bool UpdateTo097::updateData(const ConnectionInformation& ci) noexcept
{
  if (db::API::getId(ci, ApiType::Weatherstack) > 0)
  {
    // Nothing to do here.
    std::cout << "Info: API entry for Weatherstack already exists." << std::endl;
    return true;
  }

  try
  {
    db::mariadb::Connection conn(ci);
    if (conn.exec("INSERT INTO `api` (`name`, `baseURL`) VALUES ('Weatherstack', 'http://api.weatherstack.com/');") < 0)
      return false;
    // Done.
    return conn.lastInsertId() > 0;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

} // namespace
