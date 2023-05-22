/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2023  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_SEMVER_HPP
#define WEATHER_INFORMATION_COLLECTOR_SEMVER_HPP

#include <cstdint>
#include <string>

namespace wic
{

/** \brief Represents a version number in semantic versioning.
 */
class SemVer
{
  public:
    /** \brief Constructs version 0.0.0.
     */
    SemVer();


    /** \brief Constructor with preset version.
     *
     * \param major  the major version number
     * \param minor  the minor version number
     * \param patch  the patch level
     */
    SemVer(const uint16_t major, const uint16_t minor, const uint16_t patch);


    /** \brief Gets a string representation of the version number.
     *
     * \return Returns a string representation of the version number.
     */
    std::string toString() const;


    /** \brief Equality operator for SemVer.
     *
     * \param other the other SemVer to compare to
     * \return Returns true, only if this version and other are equal.
     */
    bool operator==(const SemVer& other) const;


    /** \brief Less than operator for SemVer.
     *
     * \param other the other SemVer to compare to
     * \return Returns true, only if this version is less than the other version.
     */
    bool operator<(const SemVer& other) const;
  private:
    uint16_t majorVer; /**< major version number */
    uint16_t minorVer; /**< minor version number */
    uint16_t patchVer; /**< patch level number */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_SEMVER_HPP
