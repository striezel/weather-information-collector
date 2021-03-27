/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_CONNECTION_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_CONNECTION_HPP

#include <chrono>
#include <mariadb/mysql.h>
#include "../../api/Types.hpp"
#include "../ConnectionInformation.hpp"

namespace wic::db::mariadb
{

class Result;

/** \brief Represents a connection to the database. */
class Connection
{
  public:
    /** \brief Creates a new connection to the database.
     *
     * \param ci   database connection information
     */
    Connection(const ConnectionInformation& ci);

    // No copy constructor.
    Connection(const Connection& conn) = delete;
    // No move constructor.
    Connection(Connection&& conn) = delete;
    // No assignment constructor.
    Connection& operator=(const Connection& conn) = delete;
    Connection& operator=(Connection&& conn) = delete;

    /** Destructor. Frees internal resources. */
    ~Connection();

    /** \brief Returns the raw connection handle. You should not need to use that directly.
     *
     * \return Returns the raw database connection handle.
     */
    MYSQL* raw() const;

    /** \brief Escapes a string for use in an SQL statement.
     *
     * \param str  the string to escape
     * \return Returns the properly escaped string.
     *         If escaping fails, then an exception is thrown.
     */
    std::string escape(const std::string& str) const;

    /** \brief Escapes a string and surrounds it with quotes for use in an SQL statement.
     *
     * \param str  the string to escape
     * \return Returns the properly escaped and quoted string.
     *         If escaping and quoting fails, then an exception is thrown.
     */
    std::string quote(const std::string& str) const;

    /** \brief Escapes a datetime and surrounds it with quotes for use in an SQL statement.
     *
     * \param str  the string to escape
     * \return Returns the properly escaped and quoted datetime.
     *         If escaping and quoting fails, then an exception is thrown.
     * \remarks This method is not thread-safe, i.e. multiple concurrent calls
     *          of this method will cause a data race and fail.
     */
    std::string quote(const std::chrono::time_point<std::chrono::system_clock>& dateTime) const;

    /** \brief Executes the given SQL statement and returns its result.
     *
     * \param sql   the SQL statement to execute; any special characters in the
     *              statement must be properly escaped / quoted.
     * \return Returns the result of the query.
     *         Use Result::good() to determine whether the query was successful.
     */
    Result query(const std::string& sql);

    /** \brief Executes the given SQL statement and returns the number of affected rows.
     *
     * \param sql   the SQL statement to execute; any special characters in the
     *              statement must be properly escaped / quoted.
     * \return Returns the number of affected rows for the query.
     *         Returns -1, if the query failed.
     */
    long long int exec(const std::string& sql);

    /** \brief Returns the id of the last query on a table with auto-increment
     *         id.
     *
     * \return Returns the generated id of the latest inserted row.
     *         Returns zero, if nothing was inserted.
     */
    unsigned long long int lastInsertId();

    /** \brief Returns a numerical error code of the last failed operation.
     *
     * \return Returns error code of the last operation.
     *         Returns zero, if there was no error.
     */
    unsigned int errorCode();

    /** \brief Returns error message of the last failed operation.
     *
     * \return Returns error message of the last failed operation.
     *         Returns empty string, if there was no error.
     */
    std::string errorInfo();
  private:
    MYSQL * conn;
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_CONNECTION_HPP
