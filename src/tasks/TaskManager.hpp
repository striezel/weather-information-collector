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

#ifndef WEATHER_INFORMATION_COLLECTOR_TASKMANAGER_HPP
#define WEATHER_INFORMATION_COLLECTOR_TASKMANAGER_HPP

#include "Task.hpp"

namespace wic
{

class TaskManager
{
  public:

    /** \brief character that indicates comment line in task file
     */
    static const char commentCharater;


    /** \brief loads a task from a given file
     *
     * \param fileName  name of the file where task shall be loaded from
     * \param task      Task instance that will store the loaded information
     * \return Returns true, if the task was loaded successfully.
     *         Returns false, if an error occurred.
     */
    static bool loadFromFile(const std::string& fileName, Task& task);
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_TASKMANAGER_HPP
