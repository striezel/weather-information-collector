/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector benchmark tool.
    Copyright (C) 2019  Dirk Stolle

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
#include "benchmark.hpp"
#include "../conf/Configuration.hpp"
#include "../db/ConnectionInformation.hpp"
#include "../json/JsonCppApixu.hpp"
#include "../json/JsonCppDarkSky.hpp"
#include "../json/JsonCppOwm.hpp"
#include "../json/NLohmannJsonApixu.hpp"
#include "../json/NLohmannJsonDarkSky.hpp"
#include "../json/NLohmannJsonOwm.hpp"
#include "../retrieve/SourceMySQL.hpp"
#include "../tasks/TaskManager.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void showVersion()
{
  wic::GitInfos info;
  std::cout << "weather-information-collector-benchmark, " << wic::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl;
}

void showHelp()
{
  std::cout << "weather-information-collector-benchmark [OPTIONS]\n"
            << "\n"
            << "options:\n"
            << "  -? | --help            - shows this help message\n"
            << "  -v | --version         - shows version information\n"
            << "  -c FILE | --conf FILE  - sets the file name of the configuration file to use\n"
            << "                           during the program run. If this option is omitted,\n"
            << "                           then the program will search for the configuration\n"
            << "                           in some predefined locations.\n";
}

int main(int argc, char** argv)
{
  using namespace wic;
  std::string configurationFile; /**< path of configuration file */

  if ((argc > 1) && (argv != nullptr))
  {
    for (int i = 1; i < argc; ++i)
    {
      if (argv[i] == nullptr)
      {
        std::cerr << "Error: Parameter at index " << i << " is null pointer!\n";
        return rcInvalidParameter;
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
          return rcInvalidParameter;
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
          return rcInvalidParameter;
        }
      } // if configuration file
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  // load configuration file + configured tasks
  Configuration config;
  if (!config.load(configurationFile, true, true))
  {
    std::cerr << "Error: Could not load configuration!" << std::endl;
    return rcConfigurationError;
  }

  SourceMySQL source(config.connectionInfo());

  /* ********* Weather data ********* */
  // OpenWeatherMap
  {
    int ret = weatherDataBench<JsonCppOwm, NLohmannJsonOwm>(ApiType::OpenWeatherMap, source);
    if (ret != 0)
      return ret;
  }
  // DarkSky
  {
    int ret = weatherDataBench<JsonCppDarkSky, NLohmannJsonDarkSky>(ApiType::DarkSky, source);
    if (ret != 0)
      return ret;
  }
  // Apixu
  {
    int ret = weatherDataBench<JsonCppApixu, NLohmannJsonApixu>(ApiType::Apixu, source);
    if (ret != 0)
      return ret;
  }

  /* ********* Forecast data ********* */
  // OpenWeatherMap
  {
    int ret = forecastBench<JsonCppOwm, NLohmannJsonOwm>(ApiType::OpenWeatherMap, source);
    if (ret != 0)
      return ret;
  }
  // DarkSky
  {
    int ret = forecastBench<JsonCppDarkSky, NLohmannJsonDarkSky>(ApiType::DarkSky, source);
    if (ret != 0)
      return ret;
  }
  // Apixu
  {
    int ret = forecastBench<JsonCppApixu, NLohmannJsonApixu>(ApiType::Apixu, source);
    if (ret != 0)
      return ret;
  }

  std::cout << "Done." << std::endl;
  return 0;
}