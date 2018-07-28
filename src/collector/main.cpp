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

#include <iostream>
#include <ctime>
#include "../api/Apixu.hpp"
#include "../conf/Configuration.hpp"
#include "../db/ConnectionInformation.hpp"
#include "../store/StoreMySQL.hpp"
#include "../tasks/TaskManager.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "Collector.hpp"

void showVersion()
{
  wic::GitInfos info;
  std::cout << "weather-information-collector, " << wic::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl;
}

void showHelp()
{
  std::cout << "weather-information-collector [OPTIONS]\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - shows this help message\n"
            << "  -v | --version         - shows version information\n"
            << "  -c FILE | --conf FILE  - sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n"
            << "  -l | --ignore-limits   - ignore check for API limits during startup\n";
}

int main(int argc, char** argv)
{
  std::string configurationFile; /**< path of configuration file */
  bool checkApiLimits = true; /**< whether to check if tasks exceed API limits */

  if ((argc > 1) && (argv != nullptr))
  {
    for (int i = 1; i < argc; ++i)
    {
      if (argv[i] == nullptr)
      {
        std::cerr << "Error: Parameter at index " << i << " is null pointer!\n";
        return wic::rcInvalidParameter;
      }
      const std::string param(argv[i]);
      if ((param == "-v") || (param == "--version"))
      {
        showVersion();
        return 0;
      } // if version
      else if ((param == "-?") || (param == "/?") || (param == "--help"))
      {
        showHelp();
        return 0;
      } // if help
      else if ((param == "--conf") || (param == "-c"))
      {
        if (!configurationFile.empty())
        {
          std::cerr << "Error: Configuration was already set to "
                    << configurationFile << "!" << std::endl;
          return wic::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          configurationFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param <<"\"." << std::endl;
          return wic::rcInvalidParameter;
        }
      } // if configuration file
      else if ((param == "--ignore-limits") || (param == "-l") || (param == "--do-not-check-limits"))
      {
        if (!checkApiLimits)
        {
          std::cerr << "Error: Parameter " << param << " was already specified!"
                    << std::endl;
          return wic::rcInvalidParameter;
        }
        checkApiLimits = false;
      } // if ignore limits
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return wic::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  wic::Collector collector;
  {
    // load configuration file + configured tasks
    wic::Configuration config;
    if (!config.load(configurationFile))
    {
      std::cerr << "Error: Could not load configuration!" << std::endl;
      return wic::rcConfigurationError;
    }

    // If there are no tasks, we can quit here.
    if (config.tasks().empty())
    {
      std::cerr << "Error: No collection tasks have been configured!" << std::endl;
      return wic::rcConfigurationError;
    }

    if (checkApiLimits)
    {
      if (!wic::TaskManager::withinLimits(config.tasks()))
      {
        std::cerr << "Error: The configured collection tasks would exceed the "
                  << "request limits imposed by the free API plan! If you are on"
                  << " a different plan and are sure that the configured tasks "
                  << "will not exceed the request limits imposed by the API, "
                  << "then add the parameter --ignore-limits to the call of the "
                  << "application." << std::endl;
        return wic::rcTasksExceedApiRequestLimit;
      }
    } // if check shall be performed

    if (!collector.fromConfiguration(config))
    {
      std::cerr << "Error: Could not transfer configuration data to collector!" << std::endl;
      return wic::rcConfigurationError;
    }
  } // end of scope for configuration

  /* collect() currently starts an endless loop that cannot be interrupted by
     the user, yet. Future versions might use something like signal handling
     (as in SIGINT or SIGTERM) to stop the collection.
  */
  collector.collect();

  std::cout << "Done." << std::endl;
  return 0;
}
