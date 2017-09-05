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

#ifndef WEATHER_INFORMATION_COLLECTOR_CONFIGURATION_HPP
#define WEATHER_INFORMATION_COLLECTOR_CONFIGURATION_HPP

#include <map>
#include <vector>
#include "../db/ConnectionInformation.hpp"
#include "../tasks/Task.hpp"

namespace wic
{

class Configuration
{
  public:
    /** \brief constructor
     */
    Configuration();


    /** \brief character that indicates comment line in configuration file
     */
    static const char commentCharater;


    /** \brief gets a list of potential configuration file names
     *
     * \return Returns a list of potential configuration file names.
     */
    static std::vector<std::string> potentialFileNames();


    /** \brief gets a list of potential task directory names
     *
     * \return Returns a list of potential task directory names.
     */
    static std::vector<std::string> potentialTaskDirectories();


    /** \brief gets the connection information
     *
     * \return Returns the database connection information.
     */
    const ConnectionInformation& connectionInfo() const;


    /** \brief gets the current directory for tasks
     *
     * \return Returns the current directory for tasks.
     */
    const std::string& taskDirectory() const;


    /** \brief gets the current file extension for task files
     *
     * \return Returns the current file extension for task files.
     */
    const std::string& taskExtension() const;


    /** \brief gets a container with all tasks
     *
     * \return Returns a container containing all tasks.
     */
    const std::vector<Task>& tasks() const;


    /** \brief gets the API key for the given API
     *
     * \param api   the API for which the key shall be retrieved
     * \return Returns the key for the given API, if present.
     *         Returns an empty string, if no key is known.
     */
    std::string key(const ApiType api) const;


    /** \brief Loads the configuration from a file.
     *
     * \param  fileName   file name of the configuration file
     * \param  skipTasks  whether to skip the loading of task files
     * \param  missingKeysAllowed  whether configuration file without API keys is allowed
     * \return Returns true, if configuration was loaded successfully.
     *         Returns false otherwise.
     */
    bool load(const std::string& fileName = std::string(), const bool skipTasks = false, const bool missingKeysAllowed = false);


    /** \brief clears all configuration information
     */
    void clear();
  private:
    /** \brief finds the first existing configuration file from a preset list
     *
     * \param realName  name of the configuration file, if any
     */
    void findConfigurationFile(std::string& realName);


    /** \brief loads the configuration file with "core" settings
     *
     * \param fileName  file name of the configuration file
     * \param  missingKeysAllowed  whether configuration file without API keys is allowed
     * \return Returns true, if file was loaded successfully.
     */
    bool loadCoreConfiguration(const std::string& fileName, const bool missingKeysAllowed = false);


    /** \brief finds the first existing task directory from a preset list
     *
     * \param realName  directory name, if any
     */
    void findTaskDirectory(std::string& realName);


    std::vector<Task> tasksContainer; /**< container with all tasks */
    std::map<ApiType, std::string> apiKeys; /**< API keys */
    ConnectionInformation connInfo; /**< database connection information */
    std::string tasksDirectory; /**< directory for tasks */
    std::string tasksExtension; /**< file extension (incl. dot) for task files */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_CONFIGURATION_HPP
