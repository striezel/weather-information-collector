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

#ifndef WEATHER_INFORMATION_COLLECTOR_FORECAST_HPP
#define WEATHER_INFORMATION_COLLECTOR_FORECAST_HPP

#include <chrono>
#include <string>
#include <vector>
#include "Weather.hpp"

namespace wic
{

class Forecast
{
  public:
    /** \brief Constructor. Initializes all values to empty / not set.
     */
    Forecast();


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


    /** \brief Checks whether this instance has raw JSON data.
     *
     * \return Returns true, if the instance has raw JSON data.
     *         Returns false otherwise.
     */
    bool hasJson() const;


    /** \brief Gets the raw JSON data.
     *
     * \return Returns the raw JSON data as string.
     *         Returns an empty string, if no JSON data is set.
     */
    const std::string& json() const;


    /** \brief Sets the raw JSON data.
     *
     * \param newJson  the new JSON data
     */
    void setJson(const std::string& newJson);


    /** \brief Gets the weather forecast data of this instance.
     *
     * \return Returns the forecast data. (May be empty.)
     */
    const std::vector<Weather>& data() const;


    /** \brief Sets the weather forecast data.
     *
     * \param newData  the new weather forecast data to set
     */
    void setData(const std::vector<Weather>& newData);
  private:
    std::chrono::time_point<std::chrono::system_clock> m_requestTime; /**< time when the API request was performed */
    std::string m_json; /**< raw JSON data */
    std::vector<Weather> m_forecast; /**< weather forecast data */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_FORECAST_HPP
