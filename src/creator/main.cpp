/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020, 2021, 2022, 2024  Dirk Stolle

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
#include "../api/OpenMeteo.hpp"
#include "../api/OpenWeatherMap.hpp"
#include "../conf/Configuration.hpp"
#include "../data/Location.hpp"
#include "../data/Weather.hpp"
#include "../net/curl_version.hpp"
#include "../tasks/TaskManager.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showHelp()
{
  std::cout << "weather-information-collector-task-creator [OPTIONS]\n"
            << "\n"
            << "Creates a task file for weather-information-collector, based on the given user\n"
            << "input and choices.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -c FILE | --conf FILE  - Sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n"
            << "  --open-meteo           - Use Open-Meteo geocoding API instead of the usual\n"
            << "                           OpenWeatherMap API, even if a key is available for\n"
            << "                           the later.\n";
}

std::pair<int, bool> parseArguments(const int argc, char** argv, std::string& configurationFile, bool& openMeteo)
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
      wic::showVersion("weather-information-collector-task-creator");
      showCurlVersion();
      wic::showLicenseInformation();
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
                  << configurationFile << "!\n";
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
                  << param << "\".\n";
        return std::make_pair(wic::rcInvalidParameter, true);
      }
    } // if configuration file
    else if (param == "--open-meteo")
    {
      if (openMeteo)
      {
        std::cerr << "Error: Option " << param
                  << " was specified more than once!\n";
        return std::make_pair(wic::rcInvalidParameter, true);
      }
      openMeteo = true;
    }
    else
    {
      std::cerr << "Error: Unknown parameter " << param << "!\n"
                << "Use --help to show available parameters.\n";
      return std::make_pair(wic::rcInvalidParameter, true);
    }
  } // for i

  return std::make_pair(0, false);
}

int main(int argc, char** argv)
{
  std::string configurationFile; /**< path of configuration file */
  bool forceOpenMeteo = false;

  const auto [exitCode, exitNow] = parseArguments(argc, argv, configurationFile, forceOpenMeteo);
  if (exitNow || (exitCode != 0))
    return exitCode;

  // load configuration file + configured tasks
  wic::Configuration config;
  if (!config.load(configurationFile))
  {
    std::cerr << "Error: Could not load configuration!\n";
    return wic::rcConfigurationError;
  }

  // Task creator needs an OpenWeatherMap API key to find locations.
  // If the key is missing, Open-Meteo is used instead.
  const std::string owmKey = config.key(wic::ApiType::OpenWeatherMap);

  std::cout << "This program will create a new task for the weather-information-collector.\n"
            << "It will prompt the necessary information and create a task file from that.\n"
            << std::endl;
  std::cout << "Enter the name of the location: ";
  std::string userInput;
  std::getline(std::cin, userInput);

  std::vector<std::pair<wic::Location, wic::Weather> > locations;
  bool api_success = false;
  if (!owmKey.empty() && !forceOpenMeteo)
  {
    wic::OpenWeatherMap owm;
    owm.setApiKey(owmKey);
    api_success = owm.findLocation(userInput, locations);
  }
  else
  {
    wic::OpenMeteo om;
    api_success = om.findLocation(userInput, locations);
  }
  if (!api_success || locations.empty())
  {
    std::cerr << "Could not find a location with the name \"" << userInput
              << "\". You should usually enter the name of a city.\n";
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
  wic::creator::checkApiKeyRequirement(selectedApi, config);
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
                << "new task was active.\n";
      return wic::rcTasksExceedApiRequestLimit;
    }
    else
    {
      std::cout << "WARNING: The API requests limits would be exceeded, if the"
                << " new task was active. However, the limits are already "
                << "exceeded without the new task, so it does not make a "
                << "difference." << std::endl;
    }
  }
  std::cout << "Checking for duplicate and overlapping tasks ...";
  if (!wic::TaskManager::hasDuplicates(newTasks, true))
  {
    std::cout << " OK." << std::endl;
  }
  else
  {
    std::cout << " not OK.\n";
    std::cerr << "Error: There would be duplicate or overlapping tasks, if the new"
              << " task were active. Therefore it will not be created.\n";
    return wic::rcTaskOverlap;
  }

  const std::string targetFile = wic::creator::createFileName(newTask, config);
  if (targetFile.empty())
  {
    std::cerr << "Error: Could not generate file name for new task file!\n";
    return wic::rcInputOutputError;
  }
  std::cout << "Saving new task file as " << targetFile << "." << std::endl;
  if (!wic::creator::writeTaskFile(targetFile, newTask))
  {
    std::cerr << "Error: The new task file could not be created.\n";
    return wic::rcInputOutputError;
  }

  // Success.
  std::cout << "Done. Any running instance of weather-information-collector "
            << "needs to be restarted to use the new task file, too.\n";
  return 0;
}
