/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2024  Dirk Stolle

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
#include "../db/mariadb/client_version.hpp"
#include "../db/mariadb/guess.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "UpdateTo_0.5.5.hpp"
#include "UpdateTo_0.6.0.hpp"
#include "UpdateTo_0.6.6.hpp"
#include "UpdateTo_0.7.0.hpp"
#include "UpdateTo_0.8.0.hpp"
#include "UpdateTo_0.8.1.hpp"
#include "UpdateTo_0.8.3.hpp"
#include "UpdateTo_0.8.5.hpp"
#include "UpdateTo_0.8.6.hpp"
#include "UpdateTo_0.9.0.hpp"
#include "UpdateTo_0.9.7.hpp"
#include "UpdateTo_0.9.17.hpp"
#include "UpdateTo_0.10.0.hpp"

void showHelp()
{
  std::cout << "weather-information-collector-update [OPTIONS]\n"
            << "\n"
            << "Performs database updates for a database that was created for an earlier\n"
            << "version of weather-information-collector.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - Shows this help message.\n"
            << "  -v | --version         - Shows version information.\n"
            << "  -c FILE | --conf FILE  - Sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n"
            << "  -f | --full            - Perform all update steps, even those for older\n"
            << "                           versions that may not be required. This option is\n"
            << "                           off by default, so that only required updates are\n"
            << "                           performed by the update program. Which updates are\n"
            << "                           required is determined by looking at the current\n"
            << "                           database structure.\n"  ;
}

std::pair<int, bool> parseArguments(const int argc, char** argv, std::string& configurationFile, bool& fullUpdate)
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
      wic::showVersion("weather-information-collector-update");
      wic::showMariaDbClientVersion();
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
      // Are there enough parameters?
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
    else if ((param == "--full") || (param == "-f") || (param == "--all") || (param == "-a"))
    {
      if (fullUpdate)
      {
        std::cerr << "Error: Parameter " << param << " must not be specified more than once!"
                  << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
      fullUpdate = true;
    } // if full update procedure / all updates
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
  bool fullUpdate = false;       /**< whether user wants a full update */

  const auto [exitCode, forceExit] = parseArguments(argc, argv, configurationFile, fullUpdate);
  if (forceExit || (exitCode != 0))
    return exitCode;

  // load configuration file, but skip configured tasks
  wic::Configuration config;
  if (!config.load(configurationFile, true, true))
  {
    std::cerr << "Error: Could not load configuration!" << std::endl;
    return wic::rcConfigurationError;
  }

  wic::SemVer currentVersion;
  if (!fullUpdate)
  {
    // Try to detect version from database.
    currentVersion = guessVersionFromDatabase(config.connectionInfo());
    if (wic::SemVer() == currentVersion)
    {
      // Some database error must have occurred, so quit right here.
      return wic::rcDatabaseError;
    }

    if (currentVersion == wic::ancientVersion)
    {
      std::cout << "Detected database version is very ancient, so a full update will be performed." << std::endl;
    }
    else
    {
      std::cout << "Detected database version is " << currentVersion.toString()
                << " or newer. Updates for older versions will be skipped." << std::endl;
    }
  } // if no full update is requested

  // Perform the incremental updates, step by step.
  if (currentVersion < wic::SemVer(0, 5, 5))
  {
    std::cout << "Update for database of version 0.5.4 (and earlier) to version 0.5.5..." << std::endl;
    if (!wic::UpdateTo055::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 6, 0))
  {
    std::cout << "Update for database of version 0.5.7 (and earlier) to version 0.6.0..." << std::endl;
    if (!wic::UpdateTo060::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 6, 6))
  {
    std::cout << "Update for database of version 0.6.5 (and earlier) to version 0.6.6..." << std::endl;
    if (!wic::UpdateTo066::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 7, 0))
  {
    std::cout << "Update for database of version 0.6.7 (and earlier) to version 0.7.0..." << std::endl;
    if (!wic::UpdateTo070::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 8, 0))
  {
    std::cout << "Update for database of version 0.7.1 (and earlier) to version 0.8.0..." << std::endl;
    if (!wic::UpdateTo080::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 8, 1))
  {
    std::cout << "Update for database of version 0.8.0 (and earlier) to version 0.8.1..." << std::endl;
    if (!wic::UpdateTo081::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 8, 3))
  {
    std::cout << "Update for database of version 0.8.1 (and earlier) to version 0.8.3..." << std::endl;
    if (!wic::UpdateTo083::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 8, 5))
  {
    std::cout << "Update for database of version 0.8.3 to version 0.8.5..." << std::endl;
    if (!wic::UpdateTo085::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 8, 6))
  {
    std::cout << "Update for database of version 0.8.5 (and earlier) to version 0.8.6..." << std::endl;
    if (!wic::UpdateTo086::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 9, 0))
  {
    std::cout << "Update for database of version 0.8.9 (and earlier) to version 0.9.0..." << std::endl;
    if (!wic::UpdateTo090::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 9, 7))
  {
    std::cout << "Update for database of version 0.9.6 (and earlier) to version 0.9.7..." << std::endl;
    if (!wic::UpdateTo097::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 9, 17))
  {
    std::cout << "Update for database of version 0.9.16 (and earlier) to version 0.9.17..." << std::endl;
    if (!wic::UpdateTo0917::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 10, 0))
  {
    std::cout << "Update for database of version 0.9.22 (and earlier) to version 0.10.0..." << std::endl;
    if (!wic::UpdateTo_0_10_0::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }

  std::cout << "OK. All updates succeeded." << std::endl;
  return 0;
}
