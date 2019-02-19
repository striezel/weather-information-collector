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

#include <algorithm>
#include <chrono>
#include <iostream>
#include "../conf/Configuration.hpp"
#include "../db/ConnectionInformation.hpp"
#include "../json/JsonCppOwm.hpp"
#include "../json/NLohmannJsonOwm.hpp"
#include "../retrieve/SourceMySQL.hpp"
#include "../tasks/TaskManager.hpp"
#include "../util/GitInfos.hpp"
#include "../ReturnCodes.hpp"
#include "../Version.hpp"

void printWeather(const wic::Weather& w)
{
  std::cout << "Temperature: " << w.temperatureCelsius() << " °C (" << w.hasTemperatureCelsius() << ")\n"
            << "Temperature: " << w.temperatureFahrenheit() << " °F (" << w.hasTemperatureFahrenheit() << ")\n"
            << "Temperature: " << w.temperatureKelvin() << " K (" << w.hasTemperatureKelvin() << ")\n"
            << "Humidity: " << static_cast<int>(w.humidity()) << " % (" << w.hasHumidity() << ")\n"
            << "Rain: " << w.rain() << " mm (" << w.hasRain() << ")\n"
            << "Snow: " << w.snow() << " mm (" << w.hasSnow() << ")\n"
            << "Pressure: " << w.pressure() << " hPa (" << w.hasPressure() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
  const std::time_t rt_c = std::chrono::system_clock::to_time_t(w.requestTime());
  std::cout << "Request time: " << std::ctime(&rt_c) << "\n";
}

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
  std::string configurationFile; /**< path of configuration file */

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
          return wic::rcInvalidParameter;
        }
      } // if configuration file
      else
      {
        std::cerr << "Error: Unknown parameter " << param << "!\n"
                  << "Use --help to show available parameters." << std::endl;
        return wic::rcInvalidParameter;
      }
    } // for i
  } // if arguments are there

  // load configuration file + configured tasks
  wic::Configuration config;
  if (!config.load(configurationFile, true, true))
  {
    std::cerr << "Error: Could not load configuration!" << std::endl;
    return wic::rcConfigurationError;
  }

  wic::SourceMySQL source(config.connectionInfo());
  // Find all locations with weather data.
  std::vector<std::pair<wic::Location, wic::ApiType>> weatherLocations;
  if (!source.listWeatherLocationsWithApi(weatherLocations))
  {
    std::cerr << "Error: Could not retrieve locations with weather data!" << std::endl;
    return wic::rcDatabaseError;
  }

  // Erase all locations that are not retrieved with OpenWeatherMap API.
  const auto erasableIterator =
  std::remove_if(weatherLocations.begin(), weatherLocations.end(),
              [&](const std::pair<wic::Location, wic::ApiType>& elem) { return elem.second != wic::ApiType::OpenWeatherMap; }
              );
  if (erasableIterator != weatherLocations.end())
  {
    weatherLocations.erase(erasableIterator, weatherLocations.end());
  }

  std::cout << "Locations that are parsed:\n";
  for (const auto& p : weatherLocations)
  {
    std::cout << p.first.toString() << ", " << wic::toString(p.second) << "\n";
  }
  if (weatherLocations.size() == 0)
  {
    std::cout << "none" << std::endl << std::endl << "Exiting benchmark." << std::endl;
    return 0;
  }

  std::cout << std::endl;

  for(const auto& elem : weatherLocations)
  {
    std::vector<wic::Weather> data;
    if (!source.getCurrentWeather(elem.second, elem.first, data))
    {
      std::cerr << "Error: Could not get weather data for "
                << elem.first.toString() << std::endl;
      return wic::rcDatabaseError;
    }

    // Parse with JsonCpp.
    const auto jsonCppStart = std::chrono::high_resolution_clock::now();
    for (const wic::Weather& elem: data)
    {
      if (!elem.hasJson())
        continue;

      wic::Weather dummy;
      if (!wic::JsonCppOwm::parseCurrentWeather(elem.json(), dummy))
      {
        std::cerr << "Error: Could not parse JSON with JsonCpp!" << std::endl
                  << "JSON is: '" << elem.json() << "'." << std::endl;
        return 1;
      }

      // compare with value from database
      // -- First set request time, because that one cannot be determined from
      //    the raw JSON alone.
      dummy.setRequestTime(elem.requestTime());
      if (dummy != elem)
      {
        std::cerr << "Error: JsonCpp-parsed element does not match the element from the database!" << std::endl;
        std::cerr << "json() matches: " << (elem.json() == dummy.json() ? "yes" : "no") << std::endl;
        std::cerr << "Element from database:\n";
        printWeather(elem);
        std::cerr << "Element from parser:\n";
        printWeather(dummy);
        return 42;
      }
    } // for
    const auto jsonCppEnd = std::chrono::high_resolution_clock::now();

    // Parse with nlohmann/json.
    const auto nlohmannJsonStart = std::chrono::high_resolution_clock::now();
    for (const wic::Weather& elem: data)
    {
      if (!elem.hasJson())
        continue;

      wic::Weather dummy;
      if (!wic::NLohmannJsonOwm::parseCurrentWeather(elem.json(), dummy))
      {
        std::cerr << "Error: Could not parse JSON with nlohmann/json!" << std::endl
                  << "JSON is: '" << elem.json() << "'." << std::endl;
        return 1;
      }

      // compare with value from database
      // -- First set request time, because that one cannot be determined from
      //    the raw JSON alone.
      dummy.setRequestTime(elem.requestTime());
      if (dummy != elem)
      {
        std::cerr << "Error: nlohmann/json-parsed element does not match the element from the database!" << std::endl;
        std::cerr << "json() matches: " << (elem.json() == dummy.json() ? "yes" : "no") << std::endl;
        std::cerr << "Element from database:\n";
        printWeather(elem);
        std::cerr << "Element from parser:\n";
        printWeather(dummy);
        return 42;
      }
    } // for
    const auto nlohmannJsonEnd = std::chrono::high_resolution_clock::now();

    std::cout << "Parsing " << data.size() << " elements for "
              << elem.first.toString() << " took:" << std::endl;
    const auto jsonCppTime = std::chrono::duration_cast<std::chrono::microseconds>(jsonCppEnd - jsonCppStart);
    const auto jsonCppPerElement = static_cast<double>(jsonCppTime.count()) / data.size();
    const auto nlohmannJsonTime = std::chrono::duration_cast<std::chrono::microseconds>(nlohmannJsonEnd - nlohmannJsonStart);
    const auto nlohmannJsonPerElement = static_cast<double>(nlohmannJsonTime.count()) / data.size();
    const double percentage = static_cast<double>(nlohmannJsonTime.count()) / jsonCppTime.count() * 100.0;
    std::cout << "JsonCpp:       " << jsonCppTime.count() << " microseconds (100 %)\n"
              << "               (ca. " << jsonCppPerElement << " microseconds per element)\n"
              << "nlohmann/json: " << nlohmannJsonTime.count() << " microseconds (" << percentage << " %)\n"
              << "               (ca. " << nlohmannJsonPerElement << " microseconds per element)\n"
              << std::endl;
  } // for

  std::cout << "Done." << std::endl;
  return 0;
}
