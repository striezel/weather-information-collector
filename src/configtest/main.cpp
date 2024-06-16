/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2020, 2021, 2023, 2024  Dirk Stolle

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
#include "../conf/Configuration.hpp"
#include "../db/ConnectionInformation.hpp"
#include "../tasks/TaskManager.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showHelp()
{
  std::cout << "weather-information-collector-configtest [OPTIONS]\n"
            << "\n"
            << "Checks configuration files for potential errors.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -c FILE | --conf FILE  - Sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n"
            << "  -l | --ignore-limits   - Ignore check for API limits during startup.\n";
}

std::pair<int, bool> parseArguments(const int argc, char** argv, std::string& configurationFile, bool& checkApiLimits)
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
      wic::showVersion("weather-information-collector-configtest");
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
                  << param <<"\"." << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
    } // if configuration file
    else if ((param == "--ignore-limits") || (param == "-l") || (param == "--do-not-check-limits"))
    {
      if (!checkApiLimits)
      {
        std::cerr << "Error: Parameter " << param << " was already specified!"
                  << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
      checkApiLimits = false;
    } // if help
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
  bool checkApiLimits = true; /**< whether to check if tasks exceed API limits */

  const auto [exitCode, forceExit] = parseArguments(argc, argv, configurationFile, checkApiLimits);
  if (forceExit || (exitCode != 0))
    return exitCode;

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

  if (checkApiLimits &&
      !wic::TaskManager::withinLimits(config.tasks(), config.planOpenWeatherMap(),
                                      config.planWeatherbit(), config.planWeatherstack()))
  {
    std::cerr << "Error: The configured collection tasks would exceed the "
              << "request limits imposed by the chosen API plans! If you are on"
              << " a different plan, then please adjust the configuration "
              << "file and enter your current API plans. If you are on some "
              << "custom plan and are sure that the configured tasks "
              << "will not exceed the request limits imposed by the API, "
              << "then add the parameter --ignore-limits to the call of the "
              << "application." << std::endl;
    return wic::rcTasksExceedApiRequestLimit;
  } // if check shall be performed and failed

  std::cout << "OK." << std::endl;
  return 0;
}
