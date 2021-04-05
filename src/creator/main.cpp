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

#include <iostream>
#include <utility>
#include "CliUtilities.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../conf/Configuration.hpp"
#include "../data/Location.hpp"
#include "../data/Weather.hpp"
#include "../tasks/TaskManager.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showVersion()
{
  wic::GitInfos info;
  std::cout << "weather-information-collector-task-creator, " << wic::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl
#ifdef __SIZEOF_INT128__
            << "JSON library:           simdjson" << std::endl;
#else
            << "JSON library:           nlohmann/json" << std::endl;
#endif // __SIZEOF_INT128__
}

void showHelp()
{
  std::cout << "weather-information-collector-task-creator [OPTIONS]\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - shows this help message\n"
            << "  -v | --version         - shows version information\n"
            << "  -c FILE | --conf FILE  - sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n";
}

std::pair<int, bool> parseArguments(const int argc, char** argv, std::string& configurationFile)
{
  if ((argc <= 1) || (argv == nullptr))
    return std::make_pair(0, false);

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i] == nullptr)
    {
      std::cerr << "Error: Parameter at index " << i << " is null pointer!\n";
      return std::make_pair(wic::rcInvalidParameter, true);
    }
    const std::string param(argv[i]);
    if ((param == "-v") || (param == "--version"))
    {
      showVersion();
      return std::make_pair(0, true);
    } // if version
    else if ((param == "-?") || (param == "/?") || (param == "--help"))
    {
      showHelp();
      return std::make_pair(0, true);
    } // if help
    else if ((param == "--conf") || (param == "-c"))
    {
      if (!configurationFile.empty())
      {
        std::cerr << "Error: Configuration was already set to "
                  << configurationFile << "!" << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
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
                  << param << "\"." << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
    } // if configuration file
    else
    {
      std::cerr << "Error: Unknown parameter " << param << "!\n"
                << "Use --help to show available parameters." << std::endl;
      return std::make_pair(wic::rcInvalidParameter, true);
    }
  } // for i

  return std::make_pair(0, false);
}

int main(int argc, char** argv)
{
  std::string configurationFile; /**< path of configuration file */

  const auto [exitCode, exitNow] = parseArguments(argc, argv, configurationFile);
  if (exitNow || (exitCode != 0))
    return exitCode;

  // load configuration file + configured tasks
  wic::Configuration config;
  if (!config.load(configurationFile))
  {
    std::cerr << "Error: Could not load configuration!" << std::endl;
    return wic::rcConfigurationError;
  }

  // Task creator needs an OpenWeatherMap API key to find locations.
  const std::string owmKey = config.key(wic::ApiType::OpenWeatherMap);
  if (owmKey.empty())
  {
    std::cerr << "Error: The configuration file contains no API key for OpenWeatherMap!" << std::endl;
    return wic::rcConfigurationError;
  }

  std::cout << "This program will create a new task for the weather-information-collector.\n"
            << "It will prompt the necessary information and create a task file from that.\n"
            << std::endl;
  std::cout << "Enter the name of the location: ";
  std::string userInput;
  std::getline(std::cin, userInput);

  wic::OpenWeatherMap owm;
  owm.setApiKey(owmKey);
  std::vector<std::pair<wic::Location, wic::Weather> > locations;
  if (!owm.findLocation(userInput, locations) || locations.empty())
  {
    std::cerr << "Could not find a location with the name \"" << userInput
              << "\". You should usually enter the name of a city." << std::endl;
    return wic::rcInvalidParameter;
  }
  // show matching locations
  if (locations.size() == 1)
  {
    std::cout << "Found one matching location:" << std::endl;
  }
  else
  {
    std::cout << "Found " << locations.size() << " matching locations:" << std::endl;
  }

  // Let user select the location.
  wic::Location selectedLocation;
  do
  {
    selectedLocation = wic::creator::selectLocation(locations);
  } while (selectedLocation.empty());
  // Select the API.
  wic::ApiType selectedApi = wic::ApiType::none;
  while (selectedApi == wic::ApiType::none)
  {
    selectedApi = wic::creator::selectApi();
  }
  if (config.key(selectedApi).empty())
  {
    std::cout << "WARNING: You do not currently have an API key for the "
              << "selected API in the configuration file." << std::endl;
  }
  // Select the kind of data to collect.
  wic::DataType selectedData = wic::DataType::none;
  while (selectedData == wic::DataType::none)
  {
    selectedData = wic::creator::selectDataType(selectedApi, config.planWeatherbit(), config.planWeatherstack());
  }
  // Select collection interval.
  std::chrono::seconds selectedInterval = std::chrono::seconds::zero();
  do
  {
    selectedInterval = wic::creator::selectInterval();
  } while (selectedInterval <= std::chrono::seconds::zero());

  const wic::Task newTask = wic::Task(selectedLocation, selectedApi, selectedData, selectedInterval);
  const auto& currentTasks = config.tasks();
  std::cout << "Checking conformity to API request limits ...";
  const bool currentWithinLimits = wic::TaskManager::withinLimits(currentTasks,
                      config.planOpenWeatherMap(), config.planWeatherbit(),
                      config.planWeatherstack(), true);
  std::vector<wic::Task> newTasks = currentTasks;
  newTasks.push_back(newTask);
  const bool newWithinLimits = wic::TaskManager::withinLimits(newTasks,
                      config.planOpenWeatherMap(), config.planWeatherbit(),
                      config.planWeatherstack(), true);
  if (newWithinLimits)
  {
    std::cout << " OK." << std::endl;
  }
  else
  {
    std::cout << " not OK." << std::endl;
    if (currentWithinLimits)
    {
      std::cerr << "Error: The API requests limits would be exceeded, if the "
                << "new task was active." << std::endl;
      return wic::rcTasksExceedApiRequestLimit;
    }
    if (!currentWithinLimits)
    {
      std::cout << "WARNING: The API requests limits would be exceeded, if the"
                << " new task was active. However, the limits are already "
                << "exceeded without the new task, so it does not make a "
                << "difference." << std::endl;
    }
  }
  std::cout << "Checking for duplicate and overlapping tasks ...";
  const bool newHasDuplicates = wic::TaskManager::hasDuplicates(newTasks, true);
  if (!newHasDuplicates)
  {
    std::cout << " OK." << std::endl;
  }
  else
  {
    std::cout << " not OK." << std::endl;
    std::cerr << "Error: There would be duplicate or overlapping tasks, if the new"
              << " task were active. Therefore it will not be created." << std::endl;
    return wic::rcTaskOverlap;
  }

  const std::string targetFile = wic::creator::createFileName(newTask, config);
  if (targetFile.empty())
  {
    std::cout << "Error: could not generate file name for new task file!" << std::endl;
    return wic::rcInputOutputError;
  }
  std::cout << "Saving new task file as " << targetFile << "." << std::endl;
  if (!wic::creator::writeTaskFile(targetFile, newTask))
  {
    std::cout << "Error: The new task file could not be created." << std::endl;
    return wic::rcInputOutputError;
  }

  // Success.
  std::cout << "Done. Any running instance of weather-information-collector "
            << "needs to be restarted to use the new task file, too."
            << std::endl;
  return 0;
}
