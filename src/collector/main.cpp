/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2019, 2020, 2021, 2023, 2024  Dirk Stolle

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
#include "../api/Apixu.hpp"
#include "../conf/Configuration.hpp"
#include "../db/ConnectionInformation.hpp"
#include "../db/mariadb/StoreMariaDB.hpp"
#include "../db/mariadb/client_version.hpp"
#include "../db/mariadb/guess.hpp"
#include "../net/curl_version.hpp"
#include "../tasks/TaskManager.hpp"
#include "../util/SemVer.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "Collector.hpp"

void showHelp()
{
  std::cout << "weather-information-collector [OPTIONS]\n"
            << "\n"
            << "Collects weather data according to configured collection tasks.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -c FILE | --conf FILE  - Sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n"
            << "  -l | --ignore-limits   - Ignore check for API limits during startup.\n"
            << "  --skip-update-check    - Skips the check to determine whether the database\n"
            << "                           is up to date during program startup.\n";
}

std::pair<int, bool> parseArguments(const int argc, char** argv, std::string& configurationFile, bool& checkApiLimits, bool& skipUpdateCheck)
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
      wic::showVersion("weather-information-collector");
      wic::showMariaDbClientVersion();
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
    } // if ignore limits
    else if ((param == "--skip-update-check") || (param == "--no-update-check"))
    {
      if (skipUpdateCheck)
      {
        std::cerr << "Error: Parameter " << param << " was already specified!"
                  << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
      skipUpdateCheck = true;
    } // if database update check shall be skipped
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
  bool skipUpdateCheck = false; /**< whether to skip check for up to date DB */

  const auto [exitCode, forceExit] = parseArguments(argc, argv, configurationFile, checkApiLimits, skipUpdateCheck);
  if (forceExit || (exitCode != 0))
    return exitCode;

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

    // Check whether database is up to date.
    if (!skipUpdateCheck)
    {
      wic::SemVer currentVersion = wic::guessVersionFromDatabase(config.connectionInfo());
      if (wic::SemVer() == currentVersion)
      {
        // Some database error must have occurred, so quit right here.
        std::cerr << "Error: Could not check version of database!" << std::endl;
        return wic::rcDatabaseError;
      }
      if (currentVersion < wic::mostUpToDateVersion)
      {
        const auto& ci = config.connectionInfo();
        std::cerr << "Error: The database " << ci.db() << " at " << ci.hostname()
                  << ":" << ci.port() << " seems to be from an older version of"
                  << " weather-information-collector. Please run "
                  << "weather-information-collector-update to update the database."
                  << std::endl;
        std::cerr << "If this is wrong and you want to skip that check instead,"
                  << " then call this program with the parameter --skip-update-check."
                  << " Be warned that this may result in incomplete data being "
                  << "written to the database though." << std::endl;
        return wic::rcDatabaseError;
      }
    } // if update check shall be performed
    else
    {
      // Give a warning to the user, so nobody can say "But why did nobody tell
      // me about these possible problems there?"
      std::cout << "Warning: Check whether the database is up to date has been"
                << " skipped, as requested by user. This could possibly result"
                << " in incomplete data being written to the database as well "
                << " as other database errors. Only use this if you are certain"
                << " that the database is up to date." << std::endl;
    }

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
