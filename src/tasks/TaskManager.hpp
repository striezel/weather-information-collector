/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2020  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_TASKMANAGER_HPP
#define WEATHER_INFORMATION_COLLECTOR_TASKMANAGER_HPP

#include <vector>
#include "Task.hpp"
#include "../api/Plans.hpp"

namespace wic
{

class TaskManager
{
  public:
    /** \brief character that indicates comment line in task file
     */
    static const char commentCharacter;


    /** \brief The minimum interval for requests. Smaller intervals are rejected.
     */
    static const std::chrono::seconds minimumRequestInterval;


    /** \brief Loads a task from a given file.
     *
     * \param fileName  name of the file where task shall be loaded from
     * \param task      Task instance that will store the loaded information
     * \return Returns true, if the task was loaded successfully.
     *         Returns false, if an error occurred.
     */
    static bool loadFromFile(const std::string& fileName, Task& task);


    /** \brief Loads all task files from a given directory.
     *
     * \param directory  path of the directory where task files reside
     * \param extension   extension (including dot) of file names that shall be
     *                    loaded. Empty string means all files.
     * \param storage    vector that will be used to store all loaded tasks, existing elements will be preserved
     * \param planWs     the pricing plan for Weatherstack
     * \return Returns true, if all tasks from the directory were loaded successfully.
     *         Returns false, if an error occurred.
     */
    static bool loadFromDirectory(const std::string& directory, const std::string& extension,
                                  std::vector<Task>& storage, const PlanWeatherstack planWs);


    /** \brief Checks whether a list of tasks has duplicate or overlapping tasks.
     *
     * \param tasks  vector of tasks
     * \param silent  if true, then the duplicate tasks will not be shown on
     *                standard output
     * \return Returns true, if the vector of tasks has duplicate or overlapping
     *         tasks. Returns false otherwise.
     */
    static bool hasDuplicates(const std::vector<Task>& tasks, const bool silent);


    /** \brief Checks whether the tasks can be executed within API request limits.
     *
     * \param tasks   vector of tasks
     * \param planOwm the pricing plan for OpenWeatherMap
     * \param planWb  the pricing plan for Weatherbit
     * \param planWs  the pricing plan for Weatherstack
     * \param silent  if true, then request statistics will not be shown on
     *                standard output
     * \return Returns true, if the tasks can be executed within the API request limits.
     *         Returns false otherwise.
     */
    static bool withinLimits(const std::vector<Task>& tasks,
                             const PlanOwm planOwm, const PlanWeatherbit planWb,
                             const PlanWeatherstack planWs,
                             const bool silent = false);
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_TASKMANAGER_HPP
