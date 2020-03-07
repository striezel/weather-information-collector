/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020  Dirk Stolle

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

#include "TaskManager.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <boost/filesystem.hpp>
#include "../api/Factory.hpp"
#include "../api/Limits.hpp"
#include "../util/Strings.hpp"

namespace wic
{

const char TaskManager::commentCharacter = '#';
const std::chrono::seconds TaskManager::minimumRequestInterval = std::chrono::seconds(15);

bool TaskManager::loadFromFile(const std::string& fileName, Task& task)
{
  std::ifstream stream(fileName, std::ios_base::in | std::ios_base::binary);
  if (!stream.good())
  {
    std::cerr << "Error: Could not open file " << fileName << "!" << std::endl;
    return false;
  }
  task = Task();
  std::string line;
  while (std::getline(stream, line))
  {
    trim(line);
    // skip empty lines and comment lines
    if (line.empty() || line[0] == commentCharacter)
      continue;

    // check for possible carriage return at end (happens on Windows systems)
    if (!line.empty())
    {
      if (line.at(line.length() - 1) == '\r')
      {
        line.erase(line.length() - 1);
      } // if
    }

    const auto sepPos = line.find('=');
    if (sepPos == std::string::npos)
    {
      std::cerr << "Error: Invalid line found: \"" << line <<"\".\n"
                << "General format: \"Name of Setting=value\"" << std::endl;
      return false;
    }
    std::string name = line.substr(0, sepPos);
    trim(name);
    std::string value = line.substr(sepPos);
    value.erase(0, 1);
    trim(value);
    if (value.empty())
    {
      std::cerr << "Error: Empty values are not allowed in task file "
                << fileName << "!" << std::endl;
      return false;
    }
    // test possible values
    if (name == "api")
    {
      if (task.api() != ApiType::none)
      {
        std::cerr << "Error: API type is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      const ApiType api = toApiType(value);
      if (api == ApiType::none)
      {
        std::cerr << "Error: API type \"" << value << "\" in file " << fileName
                  << " is not a recognized API!" << std::endl;
        std::cerr << "Hint: Recognized API types are:" << std::endl
                  << "\t" << toString(ApiType::Apixu) << " (deprecated, do not use anymore)" << std::endl
                  << "\t" << toString(ApiType::DarkSky) << std::endl
                  << "\t" << toString(ApiType::OpenWeatherMap) << std::endl
                  << "\t" << toString(ApiType::Weatherbit) << std::endl
                  << "\t" << toString(ApiType::Weatherstack) << std::endl;
        return false;
      }
      task.setApi(api);
      // add deprecation notice for Apixu
      if (api == ApiType::Apixu)
      {
        std::cerr << "Warning: The task in file " << fileName
                  << " uses the Apixu API which has been deprecated and shut down!"
                  << std::endl;
      }
    } // if api
    else if (name == "data")
    {
      if (task.data() != DataType::none)
      {
        std::cerr << "Error: Data type is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      const DataType data = toDataType(value);
      if (data == DataType::none)
      {
        std::cerr << "Error: Data type \"" << value << "\" in file " << fileName
                  << " is not a recognized type!" << std::endl;
        std::cerr << "Hint: Recognized data types are:" << std::endl
                  << "\t" << toString(DataType::Current) << std::endl
                  << "\t" << toString(DataType::Forecast) << std::endl
                  << "\t" << toString(DataType::CurrentAndForecast) << std::endl;
        return false;
      }
      task.setData(data);
    } // if data
    else if ((name == "interval") || (name == "interval_seconds") || (name == "seconds"))
    {
      if (task.interval() > std::chrono::seconds::zero())
      {
        std::cerr << "Error: Request interval is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      int seconds = -1;
      if (!stringToInt(value, seconds))
      {
        std::cerr << "Error: Request interval in file " << fileName
                  << " must be a positive integer value!" << std::endl;
        return false;
      }
      const std::chrono::seconds interval = std::chrono::seconds(seconds);
      if (interval < minimumRequestInterval)
      {
        std::cerr << "Error: Minimum request interval in file " << fileName
                  << " must be " << minimumRequestInterval.count() << " seconds!" << std::endl;
        return false;
      }
      task.setInterval(interval);
    } // if interval
    else if ((name == "location.id") || (name == "location_id"))
    {
      if (task.location().hasOwmId())
      {
        std::cerr << "Error: Location's OpenWeatherMap ID is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      int id = -1;
      if (!stringToInt(value, id))
      {
        std::cerr << "Error: Location's OpenWeatherMap ID in file " << fileName
                  << " must be a positive integer value!" << std::endl;
        return false;
      }
      if (id < 1)
      {
        std::cerr << "Error: Location's OpenWeatherMap ID in file " << fileName
                  << " must be a positive integer value!" << std::endl;
        return false;
      }
      Location loc(task.location());
      loc.setOwmId(id);
      task.setLocation(loc);
    } // if location.id
    else if ((name == "location.name") || (name == "location_name") || (name == "city"))
    {
      if (task.location().hasName())
      {
        std::cerr << "Error: Location name is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      if (value.size() > 200)
      {
        std::cerr << "Error: Location name in file " << fileName
                  << " is unreasonably long!" << std::endl;
        return false;
      }
      Location loc(task.location());
      loc.setName(value);
      task.setLocation(loc);
    } // if location.name
    else if ((name == "location.postcode") || (name == "location_postcode") || (name == "postcode"))
    {
      if (task.location().hasPostcode())
      {
        std::cerr << "Error: Location's postcode is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      if (value.size() > 25)
      {
        std::cerr << "Error: Location's postcode in file " << fileName
                  << " is unreasonably long!" << std::endl;
        return false;
      }
      Location loc(task.location());
      loc.setPostcode(value);
      task.setLocation(loc);
    } // if location.postcode
    else if ((name == "location.countrycode") || (name == "location_countrycode") || (name == "countrycode")
      || (name == "location.country_code") || (name == "location_country_code") || (name == "country_code"))
    {
      if (task.location().hasCountryCode())
      {
        std::cerr << "Error: Location's country code is specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      if (value.size() != 2)
      {
        std::cerr << "Error: Location's country code in file " << fileName
                  << " is not two characters long! Use the two letter"
                  << " country code from ISO-3166, please." << std::endl;
        return false;
      }
      Location loc(task.location());
      loc.setCountryCode(value);
      task.setLocation(loc);
    } // if location.countrycode
    else if ((name == "location.coordinates") || (name == "location_coordinates") || (name == "coordinates"))
    {
      if (task.location().hasCoordinates())
      {
        std::cerr << "Error: Location's coordinates are specified more than once in file "
                  << fileName << "!" << std::endl;
        return false;
      }
      const auto commaPos = value.find(',');
      if (commaPos == std::string::npos)
      {
        std::cerr << "Error: Invalid coordinates found in file " << fileName
                  << ": General format is \"latitude;longitude\", e.g. \"51.2,4.1\".\n";
        return false;
      }
      std::string latStr = value.substr(0, commaPos);
      trim(latStr);
      std::string lonStr = value.substr(commaPos);
      lonStr.erase(0, 1);
      trim(lonStr);
      float lat(-1e9);
      if (!stringToFloat(latStr, lat))
      {
        std::cerr << "Error: Latitude in file " << fileName
                  << " must be a floating point value!" << std::endl;
        return false;
      }
      if ((lat > 90.0f) || (lat < -90.0f))
      {
        std::cerr << "Error: Latitude in file " << fileName
                  << " must be within the range [-90;+90]!\n";
        return false;
      }
      float lon(-1e9);
      if (!stringToFloat(lonStr, lon))
      {
        std::cerr << "Error: Longitude in file " << fileName
                  << " must be a floating point value!" << std::endl;
        return false;
      }
      if ((lon > 180.0f) || (lon < -180.0f))
      {
        std::cerr << "Error: Longitude in file " << fileName
                  << " must be within the range [-180;+180]!" << std::endl;
        return false;
      }
      Location loc(task.location());
      loc.setCoordinates(lat, lon);
      task.setLocation(loc);
    } // if location.coordinates
    else
    {
      std::cerr << "Error while reading task file " << fileName
                << ": There is no setting named \"" << name << "\"!" << std::endl;
        return false;
    } // else (unrecognized setting name)
  } // while

  // For backwards compatibility of task files before the introduction of
  // forecast data in 0.7.0-pre, set the data type to Current, if none has
  // been given.
  if (task.data() == DataType::none)
    task.setData(DataType::Current);

  // No more data, done.
  return true;
}

bool TaskManager::loadFromDirectory(const std::string& directory, const std::string& extension,
                                    std::vector<Task>& storage, const PlanWeatherstack planWs)
{
  // Let's keep the namespace short and simple.
  namespace fs = boost::filesystem;

  fs::path dir(directory);
  try
  {
    if (!fs::exists(dir))
    {
      std::cerr << "Error: Directory " << directory << " does not exist!" << std::endl;
      return false;
    }
    if (!fs::is_directory(dir))
    {
      std::cerr << "Error: " << directory << " is not a directory!" << std::endl;
      return false;
    }

    const fs::path ext(extension);
    // Range-based for does not seem to work / compile here, so we use the "old"
    // for loop and iterator instead.
    for (fs::directory_iterator it = fs::directory_iterator(dir); it != fs::directory_iterator(); ++it)
    {
      const fs::path x = it->path();
      if (fs::is_regular_file(x))
      {
        if (extension.empty() || (x.extension() == ext))
        {
          Task t;
          if (!loadFromFile(x.string(), t))
          {
            std::cerr << "Error: Could not load task from file "
                      << x.string() << "!" << std::endl;
            return false;
          }
          if (!t.complete())
          {
            std::cerr << "Error: Task information in file " << x.string()
                      << " is incomplete!" << std::endl;
            return false;
          }
          // Check whether the specified API can handle the task.
          const auto api = Factory::create(t.api(), planWs);
          if (!api)
          {
            std::cerr << "Error: The API " << toString(t.api())
                      << " specified in the task file " << x.string()
                      << " is not supported!" << std::endl;
            return false;
          }
          // -- Location must be valid.
          if (!(api->validLocation(t.location())))
          {
            std::cerr << "Error: The location " << t.location().toString()
                      << " specified in the task file " << x.string()
                      << " is not a valid location for the API "
                      << toString(t.api()) << "!" << std::endl;
            return false;
          }
          // -- Data type must be supported.
          if (!(api->supportsDataType(t.data())))
          {
            std::cerr << "Error: The data type " << toString(t.data())
                      << " specified in the task file " << x.string()
                      << " is not supported by the API "
                      << toString(t.api()) << "!" << std::endl;
            return false;
          }
          // Tests passed, push it to the task vector.
          storage.push_back(t);
        } // if extension check passed
        else
        {
          std::clog << "Info: Skipping loading of file " << x.string()
                    << ", because it does not have the extension " << extension
                    << ".\n";
        }
      } // if regular file
    } // for
    return true;
  }
  catch (const fs::filesystem_error& ex)
  {
    std::cerr << "File system error while loading tasks from directory "
              << directory << ": " << ex.what() << std::endl;
    return false;
  }
}

bool TaskManager::hasDuplicates(const std::vector<Task>& tasks, const bool silent)
{
  const auto length = tasks.size();
  for (unsigned int i = 0; i < length; ++i)
  {
    for (unsigned int j = i + 1; j < length; ++j)
    {
      if ((tasks[i].api() == tasks[j].api())
        && (tasks[i].data() == tasks[j].data())
        && (tasks[i].location() == tasks[j].location()))
      {
        if (!silent)
        {
          std::cerr << "Error: There are duplicate / overlapping tasks for the location";
          if (tasks[i].location().hasOwmId())
            std::cerr << " with the OpenWeatherMap id " << tasks[i].location().owmId();
          if (tasks[i].location().hasName())
            std::cerr << " with the name " << tasks[i].location().name();
          if (tasks[i].location().hasPostcode())
            std::cerr << " with the postcode " << tasks[i].location().postcode();
          if (tasks[i].location().hasCoordinates())
            std::cerr << " with the coordinates " << tasks[i].location().latitude()
                      << "° N, " << tasks[i].location().longitude() << " °E";
          std::cerr << "!" << std::endl;
        }
        return true;
      } // if
    } // for j
  } // for i
  return false;
}

bool TaskManager::withinLimits(const std::vector<Task>& tasks, const PlanOwm planOwm, const PlanWeatherbit planWb,
                               const PlanWeatherstack planWs, const bool silent)
{
  std::map<ApiType, uint_least32_t> requests;
  for (const Task& t : tasks)
  {
    // Interval of zero seconds or less can never be within limit.
    if (t.interval() <= std::chrono::seconds::zero())
      return false;

    // cast duration to seconds, for better comparability
    const auto duration_seconds = std::chrono::duration_cast<std::chrono::seconds>(Limits::forApi(t.api(), planOwm, planWb, planWs).timespan);
    // Pessimistic number of requests for that task:
    //  n = limit's duration / task's interval, rounded up to the next nearest integer
    const uint_least32_t numberOfRequests = static_cast<uint_least32_t>(std::ceil(duration_seconds.count() / static_cast<double>(t.interval().count())));

    requests[t.api()] += numberOfRequests;
  } // for (range-based)

  if (!requests.empty() && !silent)
  {
    std::cout << "Info: Requests consumed via configured tasks for APIs:\n";
    for (const auto& kv : requests)
    {
      const auto l = Limits::forApi(kv.first, planOwm, planWb, planWs);
      std::cout << "    " << toString(kv.first) << ": " << kv.second << " of "
                << l.requests << " possible requests within "
                << std::chrono::duration_cast<std::chrono::seconds>(l.timespan).count()
                << " seconds\n";
    } // for (range-based)
  } // if requests are not empty

  // Check whether there are more requests than the limit allows.
  for (const auto& kv : requests)
  {
    if (kv.second > Limits::forApi(kv.first, planOwm, planWb, planWs).requests)
      return false;
  } // for (range-based)

  // Everything seems to be within the request limits.
  return true;
}

} // namespace

