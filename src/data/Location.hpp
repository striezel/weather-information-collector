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


    /** \brief sets the API id of the location
     *
     * \param newId  the new API id of the location
     */
    void setId(const uint32_t newId);


    /** \brief checks whether the location has an API id
     *
     * \return Returns true, if the location has an API id.
     *         Returns false otherwise.
     */
    bool hasId() const;


    /** \brief gets the latitude of the location
     *
     * \return Returns the latitude of the location (in degrees).
     *         Returns NaN, if no latitude is set.
     */
    float latitude() const;


    /** \brief gets the longitude of the location
     *
     * \return Returns the longitude of the location (in degrees).
     *         Returns NaN, if no longitude is set.
     */
    float longitude() const;


    /** \brief sets the latitude and longitude of the location
     *
     * \param lat  the new latitude [-90;90] of the location
     * \param lon  the new longitude [-180;180] of the location
     */
    void setLatitudeLongitude( const float lat, const float lon);


    /** \brief checks whether the location has valid latitude and longitude values
     *
     * \return Returns true, if the location has valid latitude and longitude
     *         values. Returns false otherwise.
     */
    bool hasLatitudeAndLongitude() const;


    /** \brief gets the name of the location (if any)
     *
     * \return Returns the name of the location.
     *         Returns an empty string, if no name is set.
     */
    const std::string& name() const;


    /** \brief sets the name of the location
     *
     * \param newName  the new name of the location
     */
    void setName(const std::string& newName);


    /** \brief checks whether the location has a name
     *
     * \return Returns true, if the location has a name.
     *         Returns false otherwise.
     */
    bool hasName() const;


    /** \brief gets the postcode of the location (if any)
     *
     * \return Returns the postcode of the location.
     *         Returns an empty string, if no postcode is set.
     */
    const std::string& postcode() const;


    /** \brief sets the postcode of the location
     *
     * \param newPostcode  the new postcode of the location
     */
    void setPostcode(const std::string& newPostcode);


    /** \brief checks whether the location has a postcode
     *
     * \return Returns true, if the location has a postcode.
     *         Returns false otherwise.
     */
    bool hasPostcode() const;


    /** \brief equality operator for Location
     *
     * \param other  the other location to compare to
     * \return Returns true, if both locations are equal.
     */
    bool operator==(const Location& other) const;


    /** \brief checks whether two locations have the same latitude and longitude
     *
     * \param other  the other location to compare to
     * \return Returns true, if both locations have equal latitude and longitude.
     *         If both locations have no latitude and longitude, that counts as
     *         equal, too.
     */
    bool equalLatitudeAndLongitude(const Location& other) const;
  private:
    uint32_t m_id; /**< id of the location */
    float m_latitude; /**< latitude of the location */
    float m_longitude; /**< longitude of the location */
    std::string m_name; /**< name of the location */
    std::string m_postcode; /**< postcode of the location */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DATA_LOCATION_HPP
