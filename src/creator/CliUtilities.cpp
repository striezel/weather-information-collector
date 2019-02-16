/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2019  Dirk Stolle

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

#include "CliUtilities.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include "../api/Factory.hpp"
#include "../tasks/TaskManager.hpp"
#include "../util/Strings.hpp"

namespace wic
{

namespace creator
{

double niceFloat(const float val)
{
  return std::round(val * 100.0f) / 100.0f;
}

std::string dataTypeToHumanString(const DataType dt)
{
  switch (dt)
  {
    case DataType::Current:
         return "data about the current weather";
    case DataType::Forecast:
         return "weather forecast data";
    case DataType::CurrentAndForecast:
         return "data about current weather and forecast data";
    case DataType::none:
    default:
         return "none";
  } // switch
}

void writeLocationList(const std::vector<std::pair<LocationWithCountry, Weather> >& locations)
{
  int idx = 1;
  for (const auto & elem : locations)
  {
    // index for later use
    std::cout << "[" << idx << "]: "
    // name of the location
              << elem.first.name();
    // country, if any
    if (elem.first.hasCountry())
      std::cout << ", " << elem.first.country();
    // latitude,longitude
    if (elem.first.hasCoordinates())
      std::cout << " @ " << elem.first.coordinatesToString();
    std::cout << std::endl;
    // weather
    std::cout << "     Weather: ";
    // -- temperature
    bool hasTempOutput = false;
    if (elem.second.hasTemperatureCelsius())
    {
      std::cout << niceFloat(elem.second.temperatureCelsius()) << " °C";
      hasTempOutput = true;
    }
    if (elem.second.hasTemperatureFahrenheit())
    {
      if (hasTempOutput)
        std::cout << " / ";
      std::cout << niceFloat(elem.second.temperatureFahrenheit()) << " °F";
      hasTempOutput = true;
    }
    if (elem.second.hasTemperatureKelvin())
    {
      if (hasTempOutput)
        std::cout << " / ";
      std::cout << niceFloat(elem.second.temperatureKelvin()) << " K";
    }
    // -- wind speed
    if (elem.second.hasWindSpeed())
    {
      if (hasTempOutput)
        std::cout << ", ";
      std::cout << "wind " << niceFloat(elem.second.windSpeed()) << " m/s";
      hasTempOutput = true;
    }
    if (elem.second.hasPressure())
    {
      if (hasTempOutput)
        std::cout << ", ";
      std::cout << elem.second.pressure() << " hPa";
    }
    std::cout << std::endl;
    ++idx;
  } // for (locations)
}

LocationWithCountry selectLocation(const std::vector<std::pair<LocationWithCountry, Weather> >& locations)
{
  writeLocationList(locations);
  wic::LocationWithCountry selectedLocation;
  if (locations.size() == 1)
  {
    std::cout << "Selecting " << locations[0].first.toString()
              << " automatically, because that is the only location that was"
              << " found with the given name." << std::endl;
    return locations[0].first;
  }
  else
  {
    std::cout << "Enter number of the location [1-" << locations.size() << "]: ";
    std::string userInput;
    std::getline(std::cin, userInput);
    int locationIndex = -1;
    if (!stringToInt(userInput, locationIndex))
    {
      std::cout << "Error: \"" << userInput << "\" is not a valid number." << std::endl;
    }
    else
    {
      if ((locationIndex > locations.size()) || (locationIndex < 1))
      {
        std::cout << "Error: The number must be in the range between 1 and "
                  << locations.size() << "." << std::endl;
      }
      else
      {
        return locations.at(locationIndex - 1).first;
      }
    }
  }

  // no valid selection
  return LocationWithCountry();
}

ApiType selectApi()
{
  const std::vector<ApiType> allApis = {
      ApiType::Apixu,
      ApiType::DarkSky,
      ApiType::OpenWeatherMap
  };
  std::cout << "Choose the API that shall be used for data collection:" << std::endl;
  int idx = 1;
  for (const auto api: allApis)
  {
    std::cout << "[" << idx << "]: " << toString(api) << std::endl;
    ++idx;
  }
  std::cout << "Enter the number of the API [1-" << allApis.size() << "]: ";
  std::string userInput;
  std::getline(std::cin, userInput);
  int apiIndex = -1;
  if (!stringToInt(userInput, apiIndex))
  {
    // Maybe user has entered the name directly?
    const auto api = wic::toApiType(userInput);
    if (api != ApiType::none)
      return api;
    // No, it's not an API name and not an integral number.
    std::cout << "Error: \"" << userInput << "\" is not a valid number." << std::endl;
  }
  else
  {
    if ((apiIndex > allApis.size()) || (apiIndex < 1))
    {
      std::cout << "Error: The number must be in the range between 1 and "
                << allApis.size() << "." << std::endl;
    }
    else
    {
      return allApis.at(apiIndex - 1);
    }
  }

  // No valid user input.
  return ApiType::none;
}

DataType selectDataType(const ApiType selectedApi)
{
  if (selectedApi == ApiType::none)
  {
    std::cout << "Error: An API has to be selected." << std::endl;
    return DataType::none;
  }
  const auto api = Factory::create(selectedApi, "");

  const std::vector<DataType> allDataTypes = { DataType::Current, DataType::Forecast, DataType::CurrentAndForecast };
  std::vector<DataType> supportedDataTypes;
  for (const DataType dt : allDataTypes)
  {
    if (api->supportsDataType(dt))
      supportedDataTypes.push_back(dt);
  }
  if (supportedDataTypes.size() == 0)
  {
    throw std::logic_error("API " + toString(selectedApi) + " does not support"
        + std::string(" any data types. This should not happen and might be an")
        + " indication for an incomplete implementation of that API.");
  }
  std::cout << "Choose the type of data that shall be collected:" << std::endl;
  int idx = 1;
  for (const auto dt: supportedDataTypes)
  {
    std::cout << "[" << idx << "]: " << dataTypeToHumanString(dt) << std::endl;
    ++idx;
  }
  // If there is only one possibility, select it.
  if (supportedDataTypes.size() == 1)
  {
    const auto dt = supportedDataTypes[0];
    std::cout << "Selecting " << toString(dt) << " automatically, because that"
              << " is the only supported data type of " << toString(selectedApi)
              << "." << std::endl;
    return dt;
  }

  std::cout << "Enter number of the data to be collected [1-" << supportedDataTypes.size() << "]: ";
  std::string userInput;
  std::getline(std::cin, userInput);
  int typeIndex = -1;
  if (!stringToInt(userInput, typeIndex))
  {
    // Input is not an integral number.
    std::cout << "Error: \"" << userInput << "\" is not a valid number." << std::endl;
  }
  else
  {
    if ((typeIndex > supportedDataTypes.size()) || (typeIndex < 1))
    {
      std::cout << "Error: The number must be in the range between 1 and "
                << supportedDataTypes.size() << "." << std::endl;
    }
    else
    {
      return supportedDataTypes.at(typeIndex - 1);
    }
  }

  // No valid user input.
  return DataType::none;
}

std::chrono::seconds selectInterval()
{
  const int minCount = TaskManager::minimumRequestInterval.count();
  std::cout << "Enter data collection interval in seconds (>=" << minCount << "): ";
  std::string userInput;
  std::getline(std::cin, userInput);
  int seconds = -1;
  if (!stringToInt(userInput, seconds))
  {
    // Input is not an integral number.
    std::cout << "Error: \"" << userInput << "\" is not a valid integral number." << std::endl;
    return std::chrono::seconds::zero();
  }
  if (seconds <= 0)
  {
    std::cout << "Error: Interval must be a positive integral number." << std::endl;
    return std::chrono::seconds::zero();
  }
  if (seconds < minCount)
  {
    std::cout << "Error: The interval must not be below " << minCount
              << " seconds." << std::endl;
    return std::chrono::seconds::zero();
  }

  return std::chrono::seconds(seconds);
}

std::string createFileName(const Task& t, const Configuration& conf)
{
  namespace fs = boost::filesystem;

  if (!t.complete() || conf.taskDirectory().empty())
    return std::string();

  std::string name = t.location().name() + "-" + toString(t.api()) + "-" + toString(t.data());
  std::string extension = conf.taskExtension();
  if (extension.empty())
    extension = ".task";
  if (extension[0] != '.')
    extension = "." + extension;
  // replace possibly problematic characters in file name
  {
    const std::regex unallowed("[^A-Za-z0-9\\-]");
    name = std::regex_replace(name, unallowed, "_");
  }
  // replace possibly problematic characters in extension
  {
    const std::regex unallowed("[^A-Za-z0-9\\-\\.]");
    extension = std::regex_replace(extension, unallowed, "_");
  }

  fs::path filePath = fs::path(conf.taskDirectory()) / (name + extension);
  if (!fs::exists(filePath))
    return filePath.string();
  for (int i = 2; i < 20; ++i)
  {
    filePath = fs::path(conf.taskDirectory()) / (name + "-" + std::to_string(i) + extension);
    if (!fs::exists(filePath))
      return filePath.string();
  }

  // We've tried enough variations, someone needs to clean up the task directory.
  return std::string();
}

bool writeTaskFile(const std::string& fileName, const Task& t)
{
  namespace fs = boost::filesystem;

  if (!t.complete() || fileName.empty())
    return false;

  // api
  std::string fileContent = std::string("api=") + toString(t.api()) + "\n"
  // data type
      + std::string("data=") + toString(t.data()) + "\n";
  // location
  if (t.location().hasId())
  {
    fileContent += "location.id=" + std::to_string(t.location().id()) + "\n";
  }
  if (t.location().hasName())
  {
    fileContent += "location.name=" + t.location().name() + "\n";
  }
  if (t.location().hasPostcode())
  {
    fileContent += "location.postcode=" + t.location().postcode() + "\n";
  }
  if (t.location().hasCoordinates())
  {
    fileContent += "location.coordinates=" + std::to_string(t.location().latitude()) + "," + std::to_string(t.location().longitude()) + "\n";
  }
  // interval
  fileContent += "interval=" + std::to_string(t.interval().count()) + "\n";

  // If the file already exists, then we do not want to overwrite it.
  const fs::path filePath(fileName);
  if (fs::exists(filePath))
  {
    std::cerr << "Error: A file or directory named " << fileName
              << " already exists." << std::endl;
    return false;
  }
  std::ofstream output(fileName);
  output.write(fileContent.c_str(), fileContent.size());
  if (!output.good())
  {
    output.close();
    std::cerr << "Error: Could not write to file " << fileName << "!" << std::endl;
    return false;
  }
  output.close();

  return true;
}

} // namespace

} // namespace
