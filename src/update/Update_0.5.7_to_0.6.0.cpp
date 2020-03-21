/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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

#include "Update_0.5.7_to_0.6.0.hpp"
#include <map>
#include <mysql++/mysql++.h>
#include "../api/API.hpp"
#include "../api/Types.hpp"
#include "../db/mysqlpp/API.hpp"
#include "../db/mysqlpp/Structure.hpp"

namespace wic
{

bool Update057_060::perform(const ConnectionInformation& ci)
{
  return updateData(ci);
}

bool Update057_060::updateData(const ConnectionInformation& ci)
{
  const int id = db::API::getId(ci, ApiType::DarkSky);
  if (id > 0)
  {
    // Nothing to do here.
    std::cout << "Info: API entry for DarkSky already exists." << std::endl;
    return true;
  }

  mysqlpp::Connection conn(false);
  if (!conn.connect(ci.db().c_str(), ci.hostname().c_str(), ci.user().c_str(),
                    ci.password().c_str(), ci.port()))
  {
    // Should not happen, because previous connection attempts were successful,
    // but better be safe than sorry.
    std::cerr << "Error: Could not connect to database!" << std::endl;
    return false;
  }
  mysqlpp::Query query(&conn);
  query << "INSERT INTO `api` (`name`, `baseURL`) VALUES ('DarkSky', 'https://api.darksky.net/');";
  if (!query.exec())
    return false;
  // Done.
  return query.insert_id() > 0;
}

} // namespace
