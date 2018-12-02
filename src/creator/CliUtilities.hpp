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

#ifndef WEATHER_INFORMATION_COLLECTOR_CLIUTILITIES_HPP
#define WEATHER_INFORMATION_COLLECTOR_CLIUTILITIES_HPP

#include <utility>
#include <vector>
#include "../api/Types.hpp"
#include "../conf/Configuration.hpp"
#include "../data/LocationWithCountry.hpp"
#include "../data/Weather.hpp"
#include "../tasks/Task.hpp"

namespace wic
{

namespace creator
{

/** \brief Writes a list of locations to the standard output.
 *
 * \param location  pairs of locations with corresponding weather
 */
void writeLocationList(const std::vector<std::pair<LocationWithCountry, Weather> >& locations);


/** \brief Lets the user select a location from the given list.
 *
 * \param location  pairs of locations with corresponding weather
 * \return Returns the selected location, if a valid selection was made.
 *         Returns an empty location, if no selection was made.
 */
LocationWithCountry selectLocation(const std::vector<std::pair<LocationWithCountry, Weather> >& locations);


/** \brief Lets the user select an API from the supported weather data APIs.
 *
 * \return Returns the selected API, if a valid selection was made.
 *         Returns ApiType::none, if the selection was invalid.
 */
ApiType selectApi();


/** \brief Lets the user select a data type for the selected API.
 *
 * \param selectedApi  type of the selected API from previous step
 * \return Returns the selected data type, if a valid selection was made.
 *         Returns DataType::none, if the selection was invalid.
 */
DataType selectDataType(const ApiType selectedApi);


/** \brief Lets the user select a collection interval.
 *
 * \return Returns the selected interval, if a valid selection was made.
 *         Returns std::chrono::zero(), if the selection was invalid.
 */
std::chrono::seconds selectInterval();


/** \brief Creates a file name for the newly created task.
 *
 * \param t  the new task
 * \param conf  current configuration
 * \return Returns a filename that can be used to save the task file.
 *         Returns empty string, if an error occurred.
 */
std::string createFileName(const Task& t, const Configuration& conf);


/** \brief Writes data of a task into a file.
 *
 * \param fileName  name of the file to write
 * \return Returns true, if file was created.
 *         Returns false otherwise.
 */
bool writeTaskFile(const std::string& fileName, const Task& t);

} // namespace

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_CLIUTILITIES_HPP
