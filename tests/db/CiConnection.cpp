/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#include "CiConnection.hpp"
#include "../../src/util/Environment.hpp"

namespace wic
{

ConnectionInformation getCiConn()
{
  if (isGitlabCi())
  {
    // GitLab CI configuration uses mariadb docker container with appropriate
    // environment variables.
    const std::string host = getEnvVar("MYSQL_HOST");
    return ConnectionInformation(
             host.empty() ? "mariadb" : host,
             getEnvVar("MYSQL_DATABASE"),
             getEnvVar("MYSQL_USER"),
             getEnvVar("MYSQL_PASSWORD"));
  }

  if (isGithubActions())
  {
    return ConnectionInformation(
             "localhost",
             "weather_information_collector",
             "root",
             "root");
  }

  // Assume Travis CI otherwise.
  return ConnectionInformation(
           "127.0.0.1",
           "weather_information_collector",
           "travis", // user
           "" /* Password is blank on Travis CI. */);
}

} // namespace
