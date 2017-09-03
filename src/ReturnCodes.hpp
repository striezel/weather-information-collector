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

#ifndef WEATHER_INFORMATION_COLLECTOR_RETURNCODES_HPP
#define WEATHER_INFORMATION_COLLECTOR_RETURNCODES_HPP

namespace wic
{

/** \brief exit code for invalid command line parameters */
const int rcInvalidParameter = 1;

/** \brief exit code for invalid configuration data */
const int rcConfigurationError = 2;

/** \brief exit code for when configured tasks exceed the request limit */
const int rcTasksExceedApiRequestLimit = 3;

/** \brief exit code for database schema / version update failure (update program only) */
const int rcUpdateFailure = 4;

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_RETURNCODES_HPP
