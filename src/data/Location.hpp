/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019  Dirk Stolle

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

/** \brief Holds information about a location. */
class Location
{
  public:
    /** \brief Default constructor, creates an empty location.
     */
    Location();


    /** \brief Gets the API id of the location (if any).
     *
     * \return Returns the API id of the location.
     *         Returns zero, if no id is set.
     */
    uint32_t id() const;


    /** \brief Sets the API id of the location.
     *
     * \param newId  the new API id of the location
     */
    void setId(const uint32_t newId);


    /** \brief Checks whether the location has an API id.
     *
     * \return Returns true, if the location has an API id.
     *         Returns false otherwise.
     */
    bool hasId() const;


    /** \brief Gets the latitude of the location.
     *
     * \return Returns the latitude of the location (in degrees).
     *         Returns NaN, if no latitude is set.
     */
    float latitude() const;


    /** \brief Gets the longitude of the location.
     *
     * \return Returns the longitude of the location (in degrees).
     *         Returns NaN, if no longitude is set.
     */
    float longitude() const;


    /** \brief Sets the latitude and longitude of the location.
     *
     * \param lat  the new latitude [-90;90] of the location
     * \param lon  the new longitude [-180;180] of the location
     */
    void setCoordinates(const float lat, const float lon);


    /** \brief Checks whether the location has valid latitude and longitude values.
     *
     * \return Returns true, if the location has valid latitude and longitude
     *         values. Returns false otherwise.
     */
    bool hasCoordinates() const;


    /** \brief Gets the name of the location (if any).
     *
     * \return Returns the name of the location.
     *         Returns an empty string, if no name is set.
     */
    const std::string& name() const;


    /** \brief Sets the name of the location.
     *
     * \param newName  the new name of the location
     */
    void setName(const std::string& newName);


    /** \brief Checks whether the location has a name.
     *
     * \return Returns true, if the location has a name.
     *         Returns false otherwise.
     */
    bool hasName() const;


    /** \brief Gets the postcode of the location (if any).
     *
     * \return Returns the postcode of the location.
     *         Returns an empty string, if no postcode is set.
     */
    const std::string& postcode() const;


    /** \brief Sets the postcode of the location.
     *
     * \param newPostcode  the new postcode of the location
     */
    void setPostcode(const std::string& newPostcode);


    /** \brief Checks whether the location has a postcode.
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


    /** \brief inequality operator for Location
     *
     * \param other  the other location to compare to
     * \return Returns true, if both locations are equal.
     */
    bool operator!=(const Location& other) const;


    /** \brief Checks whether two locations have the same latitude and longitude.
     *
     * \param other  the other location to compare to
     * \return Returns true, if both locations have equal latitude and longitude.
     *         If both locations have no latitude and longitude, that counts as
     *         equal, too.
     */
    bool equalCoordinates(const Location& other) const;


    /** \brief Checks whether this instance has no data at all.
     *
     * \return Returns true, if this instance has no usable data yet.
     *         Returns false otherwise.
     */
    bool empty() const;


    /** \brief Gets a human-readable representation of the location.
     *
     * \return Returns a human-readable representation of the location.
     */
    std::string toString() const;


    /** \brief Gets a human-readable representation of the location's coordinates.
     *
     * \return Returns a human-readable representation of the location's coordinates.
     */
    std::string coordinatesToString() const;
  private:
    uint32_t m_id; /**< id of the location */
    float m_latitude; /**< latitude of the location */
    float m_longitude; /**< longitude of the location */
    std::string m_name; /**< name of the location */
    std::string m_postcode; /**< postcode of the location */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DATA_LOCATION_HPP
