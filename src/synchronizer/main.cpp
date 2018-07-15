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

#include <iostream>
#include <string>
#include "../db/Utilities.hpp"
#include "../conf/Configuration.hpp"
#include "../retrieve/SourceMySQL.hpp"
#include "../store/StoreMySQL.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showVersion()
{
  wic::GitInfos info;
  std::cout << "weather-information-collector-synchronizer, " << wic::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl;
}

void showHelp()
{
  std::cout << "weather-information-collector-synchronizer [OPTIONS]\n"
            << "\n"
            << "Synchronizes data between two collector databases.\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - shows this help message\n"
            << "  -v | --version         - shows version information\n"
            << "  -c1 FILE | --src-conf FILE\n"
            << "                         - sets the file name of the configuration file that\n"
            << "                           contains the database connection settings for the\n"
            << "                           source database.\n"
            << "  -c2 FILE | --dest-conf FILE\n"
            << "                         - sets the file name of the configuration file that\n"
            << "                           contains the database connection settings for the\n"
            << "                           destination database.\n";
}

int main(int argc, char** argv)
{
  std::string srcConfigurationFile;
  std::string destConfigurationFile;
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
      else if ((param == "--src-conf") || (param == "-c1"))
      {
        if (!srcConfigurationFile.empty())
        {
          std::cerr << "Error: Source configuration was already set to "
                    << srcConfigurationFile << "!" << std::endl;
          return wic::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          srcConfigurationFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param <<"\"." << std::endl;
          return wic::rcInvalidParameter;
        }
      } //if source configuration file
      else if ((param == "--dest-conf") || (param == "-c2"))
      {
        if (!destConfigurationFile.empty())
        {
          std::cerr << "Error: Destination configuration was already set to "
                    << destConfigurationFile << "!" << std::endl;
          return wic::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          destConfigurationFile = std::string(argv[i+1]);
          // Skip next parameter, because it's already used as file path.
          ++i;
        }
        else
        {
          std::cerr << "Error: You have to enter a file path after \""
                    << param <<"\"." << std::endl;
          return wic::rcInvalidParameter;
        }
      } //if destination configuration file
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return wic::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  if (srcConfigurationFile.empty())
  {
    std::cerr << "Error: No source configuration file was specified!" << std::endl;
    return wic::rcInvalidParameter;
  }
  if (destConfigurationFile.empty())
  {
    std::cerr << "Error: No destination configuration file was specified!" << std::endl;
    return wic::rcInvalidParameter;
  }

  // load source configuration file
  wic::Configuration srcConfig;
  if (!srcConfig.load(srcConfigurationFile, true, true))
  {
    std::cerr << "Error: Could not load source configuration!" << std::endl;
    return wic::rcConfigurationError;
  }

  // load destination configuration file
  wic::Configuration destConfig;
  if (!destConfig.load(destConfigurationFile, true, true))
  {
    std::cerr << "Error: Could not load destination configuration!" << std::endl;
    return wic::rcConfigurationError;
  }

  wic::SourceMySQL dataSource = wic::SourceMySQL(srcConfig.connectionInfo());
  std::vector<std::pair<wic::Location, wic::ApiType> > locations;
  if (!dataSource.listLocationsWithApi(locations))
  {
    std::cerr << "Error: Could not load locations from source database!" << std::endl;
    return wic::rcDatabaseError;
  }
  std::cout << "Found " << locations.size() << " locations with API in the database." << std::endl;
  for(const auto& item : locations)
  {
    std::cout << "\t" << item.first.toString() << ", " << wic::toString(item.second) << std::endl;
  } // for

  wic::SourceMySQL dataDest = wic::SourceMySQL(destConfig.connectionInfo());
  std::map<wic::ApiType, int> apis;
  if (!dataDest.listApis(apis))
  {
    std::cerr << "Error: Could not load API information from destination database!" << std::endl;
    return wic::rcDatabaseError;
  }
  wic::StoreMySQL destinationStore = wic::StoreMySQL(destConfig.connectionInfo());
  for(const auto& item : locations)
  {
    std::cout << "Synchronizing data for " << item.first.toString() << ", " << wic::toString(item.second) << "..." << std::endl;
    if (apis.find(item.second) == apis.end())
    {
      std::cerr << "Error: Destination database has no API entry for "
                << wic::toString(item.second) << "!" << std::endl;
      return wic::rcDatabaseError;
    }
    const int apiId = apis[item.second];
    std::vector<wic::Weather> weather;
    if (!dataSource.getCurrentWeather(item.second, item.first, weather))
    {
      std::cerr << "Error: Could not load weather data for " << item.first.toString()
                << ", " << wic::toString(item.second) << " from source database!" << std::endl;
      return wic::rcDatabaseError;
    }
    // Get corresponding location ID in destination database.
    // If it does not exist, it will be created.
    const int locationId = dataDest.getLocationId(item.first);
    if (locationId <= 0)
    {
      std::cerr << "Error: Could find or create location " << item.first.toString()
                << ", " << wic::toString(item.second) << " in destination database!" << std::endl;
      return wic::rcDatabaseError;
    }

    // Check each data set for presence in the destination database.
    for (const wic::Weather& weatherData : weather)
    {
      if (!dataDest.hasEntry(apiId, locationId, weatherData.dataTime(), weatherData.requestTime()))
      {
        // Insert data set.
        if (!destinationStore.saveCurrentWeather(item.second, item.first, weatherData))
        {
          std::cerr << "Error: Could insert weather data into destination database!" << std::endl;
          return wic::rcDatabaseError;
        }
      } // if
    } // for
  } // for

  // All is done.
  std::cout << "Done." << std::endl;
  return 0;
}
