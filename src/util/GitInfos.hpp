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

#ifndef WEATHER_INFORMATION_COLLECTOR_GITINFOS_HPP
#define WEATHER_INFORMATION_COLLECTOR_GITINFOS_HPP

#include <string>

namespace wic
{

class GitInfos
{
  public:
    /** \brief default constructor
     */
    GitInfos();


    /** \brief gets the SHA1 hash of the current commit (40 hex digits)
     *
     * \return Returns the SHA1 hash of the current commit.
     */
    const std::string& commit() const;


    /** \brief gets date of the last commit (e.g. "2017-01-29 14:59:33 +0200")
     *
     * \return Returns the date of the last commit.
     */
    const std::string& date() const;
  private:
    std::string mCommit; /**< commit hash */
    std::string mDate; /**< time of the commit */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_GITINFOS_HPP