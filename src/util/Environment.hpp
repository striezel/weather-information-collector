/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_ENVIRONMENT_HPP
#define WEATHER_INFORMATION_COLLECTOR_ENVIRONMENT_HPP

#include <string>

namespace wic
{

/** \brief Tries to determine whether a certain environment variable is set.
 *
 * \param name the name of the environment variable
 * \return Returns true, if the environment variable is set. This may also be
 *         true for a variable that is empty but set.
 *         Returns false if the environment variable is not found.
 */
bool hasEnvVar(const std::string& name);

/** \brief Tries to get the value of a certain environment variable.
 *
 * \param name the name of the environment variable
 * \return Returns the value of th environment variable, if the environment
 *         variable is set. This may also be an empty string.
 *         Returns an empty string, if the environment variable is not found.
 * \remarks Note that an empty return value can have two different meanings:
 *          It can mean that the variable with the given name does not exist.
 *          But it can also mean that the variable is set but empty.
 *          If you want to know whether a variable is set, use the function
 *          hasEnvVar() instead.
 */
std::string getEnvVar(const std::string& name);

/** \brief Determines whether the application runs within GitLab's CI
 *         environment.
 *
 * \return Returns true, if the program runs in GitLab's CI environment.
 *         Returns false otherwise.
 */
bool isGitlabCi();

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_ENVIRONMENT_HPP
