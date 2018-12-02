/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_LOCATIONWITHCOUNTRY_HPP
#define WEATHER_INFORMATION_COLLECTOR_LOCATIONWITHCOUNTRY_HPP

#include "Location.hpp"

namespace wic
{

class LocationWithCountry: public Location
{
  public:
    /** \brief default constructor
     */
    LocationWithCountry();

    /** \brief Gets the country code of the location (if any).
     *
     * \return Returns the country of the location.
     *         Returns an empty string, if no country is set.
     */
    const std::string& country() const;

    /** \brief Sets the country of the location.
     *
     * \param newCountry  the new country of the location
     */
    void setCountry(const std::string& newCountry);

    /** \brief Checks whether the location has a country.
     *
     * \return Returns true, if the location has a country.
     *         Returns false otherwise.
     */
    bool hasCountry() const;
  private:
    std::string mCountry; /**< country code */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_LOCATIONWITHCOUNTRY_HPP
