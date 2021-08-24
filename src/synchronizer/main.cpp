/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019, 2020, 2021  Dirk Stolle

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
#include <utility>
#include "../conf/Configuration.hpp"
#include "../db/mariadb/SourceMariaDB.hpp"
#include "../db/mariadb/StoreMariaDB.hpp"
#include "../db/mariadb/StoreMariaDBBatch.hpp"
#include "../db/mariadb/Utilities.hpp"
#include "../db/mariadb/client_version.hpp"
#include "../db/mariadb/guess.hpp"
#include "../util/SemVer.hpp"
#include "../util/Strings.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

const int defaultBatchSize = 40;

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
            << "                           destination database.\n"
            << "  -b N | --batch-size N  - sets the number of records per batch insert to N.\n"
            << "                           Higher numbers mean increased performance, but it\n"
            << "                           could also result in hitting MySQL's limit for the\n"
            << "                           maximum packet size, called max_allowed_packet.\n"
            << "                           Defaults to " << defaultBatchSize << ", if no value is given.\n"
            << "  --skip-update-check    - skips the check to determine whether the databases\n"
            << "                           are up to date during program startup.\n";
}

bool isLess(const wic::WeatherMeta& lhs, const wic::Weather& rhs)
{
  return (lhs.dataTime() < rhs.dataTime())
    || ((lhs.dataTime() == rhs.dataTime()) && (lhs.requestTime() < rhs.requestTime()));
}

bool isLess(const wic::ForecastMeta& lhs, const wic::Forecast& rhs)
{
  return (lhs.requestTime() < rhs.requestTime());
}

std::pair<int, bool> parseArguments(const int argc, char** argv, std::string& srcConfigurationFile, std::string& destConfigurationFile, int& batchSize, bool& skipUpdateCheck)
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
      wic::showVersion("weather-information-collector-synchronizer");
      showMariaDbClientVersion();
      return std::make_pair(0, true);
    } // if version
    else if ((param == "-?") || (param == "/?") || (param == "--help"))
    {
      showHelp();
      return std::make_pair(0, true);
    } // if help
    else if ((param == "--src-conf") || (param == "-c1"))
    {
      if (!srcConfigurationFile.empty())
      {
        std::cerr << "Error: Source configuration was already set to "
                  << srcConfigurationFile << "!" << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
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
                  << param << "\"." << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
    } // if source configuration file
    else if ((param == "--dest-conf") || (param == "-c2"))
    {
      if (!destConfigurationFile.empty())
      {
        std::cerr << "Error: Destination configuration was already set to "
                  << destConfigurationFile << "!" << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
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
                  << param << "\"." << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
    } // if destination configuration file
    else if ((param == "--batch-size") || (param == "-b"))
    {
      if (batchSize >= 0)
      {
        std::cerr << "Error: Batch size was already set to "
                  << batchSize << "!" << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
      // enough parameters?
      if ((i+1 < argc) && (argv[i+1] != nullptr))
      {
        const std::string bsString = std::string(argv[i+1]);
        if (!wic::stringToInt(bsString, batchSize) || (batchSize <= 0))
        {
          std::cerr << "Error: Batch size must be a positive integer!"
                    << std::endl;
          return std::make_pair(wic::rcInvalidParameter, true);
        }
        if (batchSize > 1000)
        {
          std::cout << "Info: Batch size " << batchSize << " will be reduced "
                    << " to 1000, because too large batch sizes might cause "
                    << "the database server to reject inserts." << std::endl;
          batchSize = 1000;
        }
        // Skip next parameter, because it's already used as batch size.
        ++i;
      }
      else
      {
        std::cerr << "Error: You have to enter a number after \""
                  << param << "\"." << std::endl;
        return std::make_pair(wic::rcInvalidParameter, true);
      }
    } // if batch size
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
  std::string srcConfigurationFile; /**< path of configuration file for source */
  std::string destConfigurationFile; /**< path of configuration file for destination */
  int batchSize = -1; /**< number of records per batch insert */
  bool skipUpdateCheck = false; /**< whether to skip check for up to date DB */

  const auto [exitCode, forceExit] = parseArguments(argc, argv, srcConfigurationFile, destConfigurationFile, batchSize, skipUpdateCheck);
  if (forceExit || (exitCode != 0))
    return exitCode;

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

  // Check that synchronization happens between different databases.
  const auto& srcDb = srcConfig.connectionInfo();
  const auto& destDb = destConfig.connectionInfo();
  if (srcDb.port() == destDb.port() && srcDb.db() == destDb.db()
      && wic::toLowerString(srcDb.hostname()) == wic::toLowerString(destDb.hostname()))
  {
    std::cerr << "Error: Source and destination databases are identical!" << std::endl;
    return wic::rcConfigurationError;
  }

  // If there is no batch size, use the default value.
  if (batchSize < 0)
  {
    std::cout << "Info: Using default batch size value of " << defaultBatchSize
              << "." << std::endl;
    batchSize = defaultBatchSize;
  }

  // Check whether databases are up to date.
  if (!skipUpdateCheck)
  {
    for (const wic::Configuration& config : { srcConfig, destConfig })
    {
      const wic::SemVer currentVersion = wic::guessVersionFromDatabase(config.connectionInfo());
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
                  << "written to the destination database though." << std::endl;
        return wic::rcDatabaseError;
      }
    } // for
  } // if update check shall be performed
  else
  {
    // Give a warning to the user, so nobody can say "But why did nobody tell
    // me about these possible problems there?"
    std::cout << "Warning: Check whether the databases are up to date has been"
              << " skipped, as requested by user. This could possibly result"
              << " in incomplete data being written to the destination database"
              << " as well as other database errors. Only use this if you are "
              << " certain that both databases are up to date." << std::endl;
  } // else

  // Get list of location-API pairs that need to be synchronized.
  wic::SourceMariaDB dataSource = wic::SourceMariaDB(srcConfig.connectionInfo());
  std::vector<std::pair<wic::Location, wic::ApiType> > locations;
  if (!dataSource.listWeatherLocationsWithApi(locations))
  {
    std::cerr << "Error: Could not load locations from source database!" << std::endl;
    return wic::rcDatabaseError;
  }
  std::cout << "Found " << locations.size() << " locations with weather data in the database." << std::endl;
  for(const auto& [location, api] : locations)
  {
    std::cout << "\t" << location.toString() << ", " << wic::toString(api) << std::endl;
  } // for

  // Get available APIs in destination database.
  wic::SourceMariaDB dataDest = wic::SourceMariaDB(destConfig.connectionInfo());
  std::map<wic::ApiType, int> apis;
  if (!dataDest.listApis(apis))
  {
    std::cerr << "Error: Could not load API information from destination database!" << std::endl;
    return wic::rcDatabaseError;
  }

  // connection to destination database
  try
  {
    wic::db::mariadb::Connection conn(destConfig.connectionInfo());
    std::clog << "Info: Connection attempt to destination database succeeded."
              << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Could not connect to destination database: " << ex.what() << "\n";
    return wic::rcDatabaseError;
  }

  // scope for synchronization of weather data
  {
    wic::StoreMariaDBBatch destinationStore(destConfig.connectionInfo(), batchSize);
    for(const auto& [location, api] : locations)
    {
      std::cout << "Synchronizing weather data for " << location.toString()
                << ", " << wic::toString(api) << "..." << std::endl;
      if (apis.find(api) == apis.end())
      {
        std::cerr << "Error: Destination database has no API entry for "
                  << wic::toString(api) << "!" << std::endl;
        return wic::rcDatabaseError;
      }
      const int apiId = apis[api];
      std::vector<wic::Weather> sourceWeather;
      if (!dataSource.getCurrentWeather(api, location, sourceWeather))
      {
        std::cerr << "Error: Could not load weather data for " << location.toString()
                  << ", " << wic::toString(api) << " from source database!" << std::endl;
        return wic::rcDatabaseError;
      }
      // Get corresponding location ID in destination database.
      // If it does not exist, it will be created.
      const int locationId = dataDest.getLocationId(location);
      if (locationId <= 0)
      {
        std::cerr << "Error: Could find or create location " << location.toString()
                  << ", " << wic::toString(api) << " in destination database!" << std::endl;
        return wic::rcDatabaseError;
      }
      // Get existing entries in destination database.
      std::vector<wic::WeatherMeta> destinationWeatherMeta;
      if (!dataDest.getMetaCurrentWeather(api, location, destinationWeatherMeta))
      {
        std::cerr << "Error: Could not load weather data for " << location.toString()
                  << ", " << wic::toString(api) << " from destination database!" << std::endl;
        return wic::rcDatabaseError;
      }

      // Iterate over data.
      auto sourceIterator = sourceWeather.begin();
      const auto sourceEnd = sourceWeather.end();
      auto destinationIterator = destinationWeatherMeta.begin();
      const auto destinationEnd = destinationWeatherMeta.end();
      while (sourceIterator != sourceEnd)
      {
        while (destinationIterator != destinationEnd && isLess(*destinationIterator, *sourceIterator))
        {
          ++destinationIterator;
        } // while (inner)
        // Element was not found in destination, if we are at the end of the
        // container or if the dereferenced iterator is not equal to the source.
        if ((destinationIterator == destinationEnd)
            || (destinationIterator->dataTime() != sourceIterator->dataTime())
            || (destinationIterator->requestTime() != sourceIterator->requestTime()))
        {
          // Insert data set.
          if (!destinationStore.saveCurrentWeather(apiId, locationId, *sourceIterator))
          {
            std::cerr << "Error: Could insert weather data into destination database!" << std::endl;
            return wic::rcDatabaseError;
          }
        } // if
        ++sourceIterator;
      } // while
    } // range-based for (locations)
  } // scope for weather data sync

  // synchronize forecast data
  {
    if (!dataSource.listForecastLocationsWithApi(locations))
    {
      std::cerr << "Error: Could not load locations from source database!" << std::endl;
      return wic::rcDatabaseError;
    }
    std::cout << "Found " << locations.size() << " locations with forecast data in the database." << std::endl;
    for(const auto& [location, api] : locations)
    {
      std::cout << "\t" << location.toString() << ", " << wic::toString(api) << std::endl;
    } // for

    for(const auto& [location, api] : locations)
    {
      std::cout << "Synchronizing forecast data for " << location.toString()
                << ", " << wic::toString(api) << "..." << std::endl;
      std::vector<wic::Forecast> sourceForecast;
      if (!dataSource.getForecasts(api, location, sourceForecast))
      {
        std::cerr << "Error: Could not load forecast data for " << location.toString()
                  << ", " << wic::toString(api) << " from source database!" << std::endl;
        return wic::rcDatabaseError;
      }
      // Get existing entries in destination database.
      std::vector<wic::ForecastMeta> destinationForecastMeta;
      if (!dataDest.getMetaForecasts(api, location, destinationForecastMeta))
      {
        std::cerr << "Error: Could not load forecast data for " << location.toString()
                  << ", " << wic::toString(api) << " from destination database!" << std::endl;
        return wic::rcDatabaseError;
      }

      wic::StoreMariaDB destinationStore = wic::StoreMariaDB(destConfig.connectionInfo());

      // Iterate over data.
      auto sourceIterator = sourceForecast.begin();
      const auto sourceEnd = sourceForecast.end();
      auto destinationIterator = destinationForecastMeta.begin();
      const auto destinationEnd = destinationForecastMeta.end();
      while (sourceIterator != sourceEnd)
      {
        while (destinationIterator != destinationEnd && isLess(*destinationIterator, *sourceIterator))
        {
          ++destinationIterator;
        } // while (inner)
        // Element was not found in destination, if we are at the end of the
        // container or if the dereferenced iterator is not equal to the source.
        if ((destinationIterator == destinationEnd)
            || (destinationIterator->requestTime() != sourceIterator->requestTime()))
        {
          // Insert data set.
          if (!destinationStore.saveForecast(api, location, *sourceIterator))
          {
            std::cerr << "Error: Could insert forecast data into destination database!" << std::endl;
            return wic::rcDatabaseError;
          }
        } // if
        ++sourceIterator;
      } // while
    } // for (locations)
  } // scope for forecast data sync

  // All is done.
  std::cout << "Done." << std::endl;
  return 0;
}
