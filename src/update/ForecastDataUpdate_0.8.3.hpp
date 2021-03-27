/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_FORECASTDATAUPDATE_0_8_3_HPP
#define WEATHER_INFORMATION_COLLECTOR_FORECASTDATAUPDATE_0_8_3_HPP

#include <map>
#include "../api/Types.hpp"
#include "../db/ConnectionInformation.hpp"
#include "../db/mariadb/Result.hpp"

namespace wic
{

/** \brief Function object to update forecast data for version 0.8.3.
 */
class ForecastDataUpdate_083
{
  public:
    /** \brief Default constructor.
     *
     * \param _ci  database connection information
     */
    explicit ForecastDataUpdate_083(const ConnectionInformation& _ci);


    /** \brief Overloaded function operator doing the real work here.
     *
     * \param startIdx   starting index
     * \param endIdx     end index
     * \param id_to_type map that maps database id to actual API type
     * \param result     MySQL/MariaDB query result containing dataID, apiID and json for each index
     */
    void operator()(const unsigned int startIdx, const unsigned int endIdx, const std::map<int, ApiType>& id_to_type, const db::mariadb::Result& result);


    /** \brief Indicates whether the last execution failed.
     *
     * \return Returns true, if the latest execution failed.
     */
    bool failed() const;
  private:
    bool m_failed; /**< whether execution failed */
    ConnectionInformation ci; /**< database connection information */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_FORECASTDATAUPDATE_0_8_3_HPP
