/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019  Dirk Stolle

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
#include "../conf/Configuration.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"
#include "guess.hpp"
#include "Update_0.5.4_to_0.5.5.hpp"
#include "Update_0.5.7_to_0.6.0.hpp"
#include "Update_0.6.5_to_0.6.6.hpp"
#include "UpdateTo_0.7.0.hpp"
#include "UpdateTo_0.8.0.hpp"
#include "UpdateTo_0.8.1.hpp"
#include "UpdateTo_0.8.3.hpp"
#include "UpdateTo_0.8.5.hpp"
#include "UpdateTo_0.8.6.hpp"
#include "UpdateTo_0.9.0.hpp"

void showVersion()
{
  wic::GitInfos info;
  std::cout << "weather-information-collector-update, " << wic::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl;
}

void showHelp()
{
  std::cout << "weather-information-collector-update [OPTIONS]\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - shows this help message\n"
            << "  -v | --version         - shows version information\n"
            << "  -c FILE | --conf FILE  - sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n"
            << "  -f | --full            - perform all update steps, even those for older\n"
            << "                           versions that may not be required. This option is\n"
            << "                           off by default, so that only required updates are\n"
            << "                           performed by the update program. Which updates are\n"
            << "                           required is determined by looking at the current\n"
            << "                           database structure.\n"  ;
}

int main(int argc, char** argv)
{
  std::string configurationFile; /**< path of configuration file */
  bool fullUpdate = false;       /**< whether user wants a full update */

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
          return wic::rcInvalidParameter;
        }
      } // if configuration file
      else if ((param == "--full") || (param == "-f") || (param == "--all") || (param == "-a"))
      {
        if (fullUpdate)
        {
          std::cerr << "Error: Parameter " << param << " must not be specified more than once!"
                    << std::endl;
          return wic::rcInvalidParameter;
        }
        fullUpdate = true;
      } // if full update procedure / all updates
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return wic::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

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
      std::cout << "Detected program version is very ancient, so a full update will be performed." << std::endl;
    }
    else
    {
      std::cout << "Detected program version is " << currentVersion.toString()
                << " or newer. Updates for older versions will be skipped." << std::endl;
      std::cout << "If this is wrong and you want to perform all update steps "
                << "instead, then call this program with the parameter --full."
                << " Be warned that this may take quite a long time though." << std::endl;
    }
  } // if no full update is requested

  // Perform the incremental updates, step by step.
  if (currentVersion < wic::SemVer(0, 5, 5))
  {
    std::cout << "Update for database of version 0.5.4 (and earlier) to version 0.5.5..." << std::endl;
    if (!wic::Update054_055::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 6, 0))
  {
    std::cout << "Update for database of version 0.5.7 (and earlier) to version 0.6.0..." << std::endl;
    if (!wic::Update057_060::perform(config.connectionInfo()))
    {
      std::cerr << "Error: Database update failed!" << std::endl;
      return wic::rcUpdateFailure;
    }
  }
  if (currentVersion < wic::SemVer(0, 6, 6))
  {
    std::cout << "Update for database of version 0.6.5 (and earlier) to version 0.6.6..." << std::endl;
    if (!wic::Update065_066::perform(config.connectionInfo()))
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
    std::cout << "Update for database of version 0.8.3 (and earlier) to version 0.8.5..." << std::endl;
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

  std::cout << "OK. All updates succeeded." << std::endl;
  return 0;
}
