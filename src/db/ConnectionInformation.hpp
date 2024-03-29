/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_CONNECTIONINFORMATION_HPP
#define WEATHER_INFORMATION_COLLECTOR_CONNECTIONINFORMATION_HPP

#include <cstdint>
#include <string>
#include <string_view>

namespace wic
{

/** \brief Holds information to connect to a database server.
 */
class ConnectionInformation
{
  public:
    /** \brief default port number for MariaDB database servers
     */
    static const uint16_t defaultMariaDbPort;


    /** \brief Default constructor.
     */
    ConnectionInformation();


    /** \brief Constructor.
     *
     * \param host  hostname of the database server
     * \param db    database name
     * \param user  name of the database user
     * \param pass  password for the database user
     * \param port  port number of the database server
     */
    ConnectionInformation(const std::string& host, const std::string& db, const std::string& user, const std::string& pass, const uint16_t port = defaultMariaDbPort);


    /** \brief Gets the hostname of the database server.
     *
     * \return Returns the hostname of the database server.
     */
    const std::string& hostname() const;


    /** \brief Sets the hostname for the database connection.
     *
     * \param host  the new hostname
     */
    void setHostname(const std::string_view host);


    /** \brief Gets the database name.
     *
     * \return Returns the database name.
     */
    const std::string& db() const;


    /** \brief Sets the database name for the database connection.
     *
     * \param database  the new database name
     */
    void setDb(const std::string_view database);


    /** \brief Gets the name of the database user.
     *
     * \return Returns the name of the database user.
     */
    const std::string& user() const;


    /** \brief Sets the name of the database user.
     *
     * \param username  the new user
     */
    void setUser(const std::string_view username);


    /** \brief Gets the password for the database user.
     *
     * \return Returns the password for the database user.
     */
    const std::string& password() const;


    /** \brief Sets the password for the database user.
     *
     * \param pw  the new password
     */
    void setPassword(const std::string_view pw);


    /** \brief Gets the port of the database server.
     *
     * \return Returns the port of the database server.
     */
    uint16_t port() const;


    /** \brief Sets the port of the database server.
     *
     * \param p  the new port number
     */
    void setPort(const uint16_t p);


    /** \brief Checks whether the connection information is complete.
     *
     * \return Returns true, if the object has all information required to
     *         connect to a MariaDB server. Returns false otherwise.
     */
    bool isComplete() const;


    /** \brief Clears all information from this object.
     */
    void clear();
  private:
    std::string db_host; /**< hostname of the SQL server */
    std::string db_name; /**< database name */
    std::string db_user; /**< database user */
    std::string db_pass; /**< password for the database user */
    uint16_t db_port; /**< port number of the database server */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_CONNECTIONINFORMATION_HPP
