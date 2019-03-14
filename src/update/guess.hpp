/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_GUESSVERSION_HPP
#define WEATHER_INFORMATION_COLLECTOR_GUESSVERSION_HPP

#include "../db/ConnectionInformation.hpp"
#include "SemVer.hpp"

namespace wic
{

const SemVer ancientVersion(0, 0, 1); /**< dummy version for very old versions */

/** \brief Tries to guess the current program version from the database structure.
 *
 * Note that the actual version may be newer.
 * \param ci  database connection information
 * \return Returns the detected version.
 */
SemVer guessVersionFromDatabase(const ConnectionInformation& ci);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_GUESSVERSION_HPP
