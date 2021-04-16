/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_EXCPECTIONS_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_EXCPECTIONS_HPP

#include <new>       // std::bad_alloc
#include <stdexcept> // std::runtime_error
#include <string>

namespace wic::db
{

/** \brief Exception that gets thrown when no database handle can be allocated.
 */
class AllocationFailure: public std::bad_alloc
{
  public:
    /// Default constructor.
    AllocationFailure() = default;


    /** \brief Returns an explanatory string for the exception.
     *
     * \return Returns the explanation.
     */
    const char* what() const noexcept final;
}; // class


/// Exception that gets thrown when connection to the DB cannot be established.
class ConnectionFailure: public std::runtime_error
{
  public:
    /** \brief Creates a new exception with the given error reason.
     *
     * \param reason   a string explaining why the connection failed
     */
    ConnectionFailure(const std::string& reason);
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_EXCPECTIONS_HPP
