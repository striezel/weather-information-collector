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

#ifndef WEATHER_INFORMATION_COLLECTOR_TASK_HPP
#define WEATHER_INFORMATION_COLLECTOR_TASK_HPP

#include <chrono>
#include "../api/Types.hpp"
#include "../data/Location.hpp"

namespace wic
{

/** \brief holds information about a collection task */
class Task
{
  public:
    /** \brief default constructor
     *
     * \param loc       location of the task
     * \param a         the API type to use for requests
     * \param _interval interval between two consecutive API requests
     */
    Task(const Location& loc = Location(), const ApiType a = ApiType::none, const std::chrono::seconds& _interval = std::chrono::seconds::zero());


    /** \brief gets the location of the collection task
     *
     * \return Returns the location of the collection task.
     */
    const Location& location() const;


    /** \brief sets a new location for the task
     *
     * \param loc  the new location of the task
     */
    void setLocation(const Location& loc);


    /** \brief gets the type of the API that is used for requests
     *
     * \return Returns the type of the API that is used for requests.
     */
    ApiType api() const;


    /** \brief sets a new API for the task
     *
     * \param a  the new API of the task
     */
    void setApi(const ApiType a);


    /** \brief gets the interval between two consecutive API requests
     *
     * \return Returns the interval between two consecutive API requests.
     */
    std::chrono::seconds interval() const;


    /** \brief sets a new interval between two consecutive API requests
     *
     * \param _interval  the new interval
     */
    void setInterval(const std::chrono::seconds& _interval);


    /** \brief checks whether this instance has all required information
     *
     * \return Returns true, if all required data is set.
     */
    bool complete() const;
  private:
    Location m_loc; /**< location for which data will be collected */
    ApiType m_api; /**< API that is used for requests */
    std::chrono::seconds m_interval; /**< interval between two consecutive API requests */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_TASK_HPP
