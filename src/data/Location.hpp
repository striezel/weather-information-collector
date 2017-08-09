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

#ifndef WEATHER_INFORMATION_COLLECTOR_DATA_LOCATION_HPP
#define WEATHER_INFORMATION_COLLECTOR_DATA_LOCATION_HPP

#include <cstdint>
#include <string>

namespace wic
{

/** \brief holds information about a location */
class Location
{
  public:
    /** \brief default constructor
     */
    Location();


    /** \brief gets the API id of the location (if any)
     *
     * \return Returns the API id of the location.
     *         Returns zero, if no id is set.
     */
    uint32_t id() const;


    /** \brief gets the longitude of the location
     *
     * \return Returns the longitude of the location (in degrees).
     *         Returns NaN, if no longitude is set.
     */
    float longitude() const;


    /** \brief gets the latitude of the location
     *
     * \return Returns the latitude of the location (in degrees).
     *         Returns NaN, if no latitude is set.
     */
    float latitude() const;


    /** \brief gets the name of the location (if any)
     *
     * \return Returns the name of the location.
     *         Returns an empty string, if no name is set.
     */
    const std::string& name() const;


    /** \brief gets the postcode of the location (if any)
     *
     * \return Returns the postcode of the location.
     *         Returns an empty string, if no postcode is set.
     */
    const std::string& postcode() const;
  private:
    uint32_t m_id; /**< id of the location */
    float m_longitude; /**< longitude of the location */
    float m_latitude; /**< latitude of the location */
    std::string m_name; /**< name of the location */
    std::string m_postcode; /**< postcode of the location */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DATA_LOCATION_HPP
