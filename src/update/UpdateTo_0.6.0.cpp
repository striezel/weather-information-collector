/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020, 2021  Dirk Stolle

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

#include "UpdateTo_0.6.0.hpp"
#include <iostream>
#include "../api/API.hpp"
#include "../api/Types.hpp"
#include "../db/mariadb/API.hpp"
#include "../db/mariadb/Structure.hpp"

namespace wic
{

bool UpdateTo060::perform(const ConnectionInformation& ci) noexcept
{
  return updateData(ci);
}

bool UpdateTo060::updateData(const ConnectionInformation& ci) noexcept
{
  if (db::API::getId(ci, ApiType::DarkSky) > 0)
  {
    // Nothing to do here.
    std::cout << "Info: API entry for DarkSky already exists." << std::endl;
    return true;
  }

  try
  {
    db::mariadb::Connection conn(ci);
    if (conn.exec("INSERT INTO `api` (`name`, `baseURL`) VALUES ('DarkSky', 'https://api.darksky.net/');") < 0)
      return false;
    // Done.
    return conn.lastInsertId() > 0;
  }
  catch (const std::exception& ex)
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database! " << ex.what() << std::endl;
    return false;
  }
}

} // namespace
