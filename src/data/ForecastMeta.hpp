/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_FORECASTMETA_HPP
#define WEATHER_INFORMATION_COLLECTOR_FORECASTMETA_HPP

#include <chrono>
#include <string>

namespace wic
{

/** \brief Contains metadata about weather forecast information.
 */
class ForecastMeta
{
  public:
    /** \brief Constructor. Initializes all values to empty / not set.
     */
    ForecastMeta();


    /** \brief Checks whether this instance has time when the API request was performed.
     *
     * \return Returns true, if the instance has the time when then API request was performed.
     *         Returns false otherwise.
     */
    bool hasRequestTime() const;


    /** \brief Sets the time when the API request was performed.
     *
     * \param rt  the new time when the API request was performed
     */
    void setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt);


    /** \brief Gets the time when the API request was performed.
     *
     * \return Returns the time when the API request was performed.
     *         Returns the epoch, if no time is set.
     */
    const std::chrono::time_point<std::chrono::system_clock>& requestTime() const;
  private:
    std::chrono::time_point<std::chrono::system_clock> m_requestTime; /**< time when the API request was performed */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_FORECASTMETA_HPP
