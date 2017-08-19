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

#include "ConnectionInformation.hpp"

namespace wic
{

const uint16_t ConnectionInformation::defaultMySqlPort = 3306;

ConnectionInformation::ConnectionInformation()
: db_host("localhost"),
  db_name("database"),
  db_user("root"),
  db_pass(""),
  db_port(defaultMySqlPort)
{
}

ConnectionInformation::ConnectionInformation(const std::string& host, const std::string& db, const std::string& user, const std::string& pass, const uint16_t port)
: db_host(host),
  db_name(db),
  db_user(user),
  db_pass(pass),
  db_port(port)
{
}

const std::string& ConnectionInformation::hostname() const
{
  return db_host;
}

const std::string& ConnectionInformation::db() const
{
  return db_name;
}

const std::string& ConnectionInformation::user() const
{
  return db_user;
}

const std::string& ConnectionInformation::password() const
{
  return db_pass;
}

uint16_t ConnectionInformation::port() const
{
  return db_port;
}

bool ConnectionInformation::isComplete() const
{
  /* Basically we need a hostname, db name, user and port must not be zero.
     Password is not strictly required, because some database configurations
     allow a connection even without a password. (Little do they know...)
  */
  return !db_host.empty() && !db_name.empty() && !db_user.empty() && (db_port != 0);
}

} //namespace
