/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_COLLECTOR_HPP
#define WEATHER_INFORMATION_COLLECTOR_COLLECTOR_HPP

#include "../api/API.hpp"
#include "../conf/Configuration.hpp"

namespace wic
{

/**
 * Auxiliary structure for class Collector.
 */
struct TaskData
{
  /** \brief Constructor.
   *
   * \param t   the task
   * \param tp  time of the next request
   */
  TaskData(const Task& t, const std::chrono::steady_clock::time_point& tp);

  Task task; /**< task data */
  std::chrono::steady_clock::time_point nextRequest; /**< time of next request */
}; // struct

/**
 * Class that handles weather data collection process.
 */
class Collector
{
  public:
    /** Default constructor. */
    Collector();


    /** \brief Gets all required information from the configuration.
     *
     * \param conf  the configuration
     * \return Returns true in case of success, returns false otherwise.
     *         Failure usually means that the configuration was not loaded.
     */
    bool fromConfiguration(const Configuration& conf);


    /** \brief Starts data collection.
     */
    void collect();


    /** \brief Asks the collector to stop the data collection.
     *
     * \remarks Calling this method might not stop the data collection
     *          immediately, it can take until the next collection is finished.
     */
    void stop();


    /** \brief Checks whether someone requested to stop the data collection.
     *
     * \return Returns true, if stop of data collection was requested.
     */
    bool stopRequested() const;
  private:
    /** \brief Gets the index of the next task that shall be executed.
     *
     * \return Returns index of the next collection task to be executed.
     *         Returns -1, if the task list is empty.
     */
    int_least32_t nextIndex() const;


    /** \brief Handles data collection and storage for current weather.
     *
     * \param api   the API implementation that is used for collection
     * \param type  enumeration that indicates the API type
     * \param loc   location for which the data will be collected
     */
    void collectCurrent(API& api, const ApiType type, const Location& loc) const;


    /** \brief Handles data collection and storage for forecast data.
     *
     * \param api   the API implementation that is used for collection
     * \param type  enumeration that indicates the API type
     * \param loc   location for which the data will be collected
     */
    void collectForecast(API& api, const ApiType type, const Location& loc) const;


    /** \brief Handles data collection and storage for current + forecast data.
     *
     * \param api   the API implementation that is used for collection
     * \param type  enumeration that indicates the API type
     * \param loc   location for which the data will be collected
     */
    void collectCurrentAndForecast(API& api, const ApiType type, const Location& loc) const;


    std::vector<TaskData> tasksContainer; /**< container with all tasks */
    std::map<ApiType, std::string> apiKeys; /**< API keys */
    ConnectionInformation connInfo; /**< database connection information */
    PlanWeatherbit planWb; /**< used pricing plan for Weatherbit */
    PlanWeatherstack planWs; /**< used pricing plan for Weatherstack */
    bool stopFlag; /**< set to true, when application requested to stop collection */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_COLLECTOR_HPP
