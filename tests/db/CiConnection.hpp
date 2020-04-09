/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
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

#ifndef WEATHER_INFORMATION_COLLECTOR_CICONNECTION_HPP_INCLUDED
#define WEATHER_INFORMATION_COLLECTOR_CICONNECTION_HPP_INCLUDED

#include "../../src/db/ConnectionInformation.hpp"

namespace wic
{

/** \brief Gets the database connection information used in the CI environment.
 *         Note that the information may be incomplete outside of the CI
 *         environment and shall not be used.
 *
 * \return Returns the database connection information used in the CI environment.
 */
ConnectionInformation getCiConn();

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_CICONNECTION_HPP_INCLUDED
