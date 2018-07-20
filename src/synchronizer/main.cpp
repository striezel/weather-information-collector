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
#include "../store/StoreMySQLBatch.hpp"
#include "../util/GitInfos.hpp"
#include "../util/Strings.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

const int defaultBatchSize = 125;

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
            << "                           destination database.\n"
            << "  -b N | --batch-size N  - sets the number of records per batch insert to N.\n"
            << "                           Higher numbers mean increased performance, but it\n"
            << "                           could also result in hitting MySQL's limit for the\n"
            << "                           maximum packet size, called max_allowed_packet.\n"
            << "                           Defaults to " << defaultBatchSize << ", if no value is given.\n" ;
}

bool isLess(const wic::Weather& lhs, const wic::Weather& rhs)
{
  return (lhs.dataTime() < rhs.dataTime())
    || ((lhs.dataTime() == rhs.dataTime()) && (lhs.requestTime() < rhs.requestTime()));
}

int main(int argc, char** argv)
{
  std::string srcConfigurationFile;
  std::string destConfigurationFile;
  int batchSize = -1;
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
                    << param << "\"." << std::endl;
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
                    << param << "\"." << std::endl;
          return wic::rcInvalidParameter;
        }
      } //if destination configuration file
      else if ((param == "--batch-size") || (param == "-b"))
      {
        if (batchSize >= 0)
        {
          std::cerr << "Error: Batch size was already set to "
                    << batchSize << "!" << std::endl;
          return wic::rcInvalidParameter;
        }
        // enough parameters?
        if ((i+1 < argc) && (argv[i+1] != nullptr))
        {
          const std::string bsString = std::string(argv[i+1]);
          if (!wic::stringToInt(bsString, batchSize) || (batchSize <=0 ))
          {
            std::cerr << "Error: Batch size must be a positive integer!"
                      << std::endl;
            return wic::rcInvalidParameter;
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
  // If there is no batch size, use 25 as default value.
  if (batchSize < 0)
  {
    std::cout << "Info: Using default batch size value of " << defaultBatchSize
              << "." << std::endl;
    batchSize = defaultBatchSize;
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

  // Get list of location-API pairs that need to be synchronized.
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

  // Get available APIs in destination database.
  wic::SourceMySQL dataDest = wic::SourceMySQL(destConfig.connectionInfo());
  std::map<wic::ApiType, int> apis;
  if (!dataDest.listApis(apis))
  {
    std::cerr << "Error: Could not load API information from destination database!" << std::endl;
    return wic::rcDatabaseError;
  }

  // connection to destination database
  mysqlpp::Connection destinationConn(false);
  if (!destinationConn.connect(destConfig.connectionInfo().db().c_str(),
                               destConfig.connectionInfo().hostname().c_str(),
                               destConfig.connectionInfo().user().c_str(),
                               destConfig.connectionInfo().password().c_str(),
                               destConfig.connectionInfo().port()))
  {
    std::cerr << "Could not connect to destination database: " << destinationConn.error() << "\n";
    return wic::rcDatabaseError;
  }
  wic::StoreMySQLBatch destinationStore = wic::StoreMySQLBatch(destConfig.connectionInfo(), batchSize);
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
    std::vector<wic::Weather> sourceWeather;
    if (!dataSource.getCurrentWeather(item.second, item.first, sourceWeather))
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
    // Get existing entries in destination database.
    std::vector<wic::Weather> destinationWeather;
    if (!dataDest.getCurrentWeather(item.second, item.first, destinationWeather))
    {
      std::cerr << "Error: Could not load weather data for " << item.first.toString()
                << ", " << wic::toString(item.second) << " from destination database!" << std::endl;
      return wic::rcDatabaseError;
    }

    // Iterate over data.
    auto sourceIterator = sourceWeather.begin();
    const auto sourceEnd = sourceWeather.end();
    auto destinationIterator = destinationWeather.begin();
    auto destinationEnd = destinationWeather.end();
    while (sourceIterator != sourceEnd)
    {
      while (destinationIterator != destinationEnd && isLess(*destinationIterator, *sourceIterator))
      {
        ++destinationIterator;
      } // while (inner)
      // Element was not found in destination. if we are at the end of the
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

  // All is done.
  std::cout << "Done." << std::endl;
  return 0;
}
