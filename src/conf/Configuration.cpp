/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2025  Dirk Stolle

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

#include "Configuration.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#ifndef wic_no_tasks_in_config
#include <algorithm>
#include "../tasks/TaskManager.hpp"
#endif // wic_no_tasks_in_config
#include "../util/Directories.hpp"
#include "../util/Strings.hpp"

namespace wic
{

// use same comment character as in task files: '#'
const char Configuration::commentCharacter = '#';

Configuration::Configuration()
:
  #ifndef wic_no_tasks_in_config
  tasksContainer(std::vector<Task>()),
  #endif // wic_no_tasks_in_config
  apiKeys(std::map<ApiType, std::string>()),
  planOwm(PlanOwm::none),
  planWb(PlanWeatherbit::none),
  planWs(PlanWeatherstack::none),
  connInfo(ConnectionInformation("", "", "", "", 0)),
  tasksDirectory(""),
  tasksExtension("")
{
}

std::vector<std::string> Configuration::potentialFileNames()
{
  std::vector<std::string> result = {
      "/etc/weather-information-collector/weather-information-collector.conf",
      "/etc/weather-information-collector/wic.conf",
      "/etc/wic/wic.conf",
      "./weather-information-collector.conf",
      "./wic.conf"
  };
  std::string home;
  if (filesystem::getHome(home))
  {
    const auto delim = filesystem::pathDelimiter;
    result.insert(result.begin(), home + delim + ".weather-information-collector" + delim + "weather-information-collector.conf");
    result.insert(result.begin(), home + delim + ".weather-information-collector" + delim + "wic.conf");
    result.insert(result.begin(), home + delim + ".wic" + delim + "wic.conf");
  }
  return result;
}

std::vector<std::string> Configuration::potentialTaskDirectories()
{
  std::vector<std::string> result = {
      "/etc/weather-information-collector/task.d",
      "/etc/wic/task.d",
      "./task.d"
  };
  std::string home;
  if (filesystem::getHome(home))
  {
    const auto delim = filesystem::pathDelimiter;
    result.insert(result.begin(), home + delim + ".weather-information-collector" + delim + "task.d");
    result.insert(result.begin(), home + delim + ".wic" + delim + "task.d");
  }
  return result;
}

const ConnectionInformation& Configuration::connectionInfo() const
{
  return connInfo;
}

#ifndef wic_no_tasks_in_config
const std::string& Configuration::taskDirectory() const
{
  return tasksDirectory;
}

const std::string& Configuration::taskExtension() const
{
  return tasksExtension;
}

const std::vector<Task>& Configuration::tasks() const
{
  return tasksContainer;
}
#endif // wic_no_tasks_in_config

std::string Configuration::key(const ApiType api) const
{
  const auto iter = apiKeys.find(api);
  if (iter != apiKeys.end())
    return iter->second;
  else
    return std::string();
}

PlanOwm Configuration::planOpenWeatherMap() const
{
  return planOwm;
}

PlanWeatherbit Configuration::planWeatherbit() const
{
  return planWb;
}

PlanWeatherstack Configuration::planWeatherstack() const
{
  return planWs;
}

void Configuration::findConfigurationFile(std::string& realName)
{
  namespace fs = std::filesystem;

  if (!realName.empty())
    return;

  for(const auto& file : potentialFileNames())
  {
    const fs::path path(file);
    try
    {
      if (fs::exists(path) && fs::is_regular_file(path))
      {
        realName = file;
        std::cout << "Info: Using configuration file " << file
                  << ", because none has been specified explicitly." << std::endl;
        break;
      }
    }
    catch (const fs::filesystem_error& ex)
    {
      std::cerr << "File system error while checking existence of file "
                << file << ": " << ex.what() << std::endl;
    } // try-catch
  } // for
}

void Configuration::findTaskDirectory(std::string& realName)
{
  namespace fs = std::filesystem;

  if (!realName.empty())
    return;

  for(const auto& dir : potentialTaskDirectories())
  {
    const fs::path path(dir);
    try
    {
      if (fs::exists(path) && fs::is_directory(path))
      {
        realName = dir;
        std::cout << "Info: Using task directory " << dir << ", because none"
                  << " has been specified explicitly." << std::endl;
        break;
      }
    }
    catch (const fs::filesystem_error& ex)
    {
      std::cerr << "File system error while checking existence of directory "
                << dir << ": " << ex.what() << std::endl;
    } // try-catch
  } // for
}

bool Configuration::loadCoreConfigurationValue(const std::string& name, const std::string& value, const std::string& fileName)
{
  if (value.empty() && (name != "task.extension") && (name != "tasks.extension"))
  {
    // Only useful empty value is the file extension for task files (empty
    // extension means all files are matched).
    // Someone might have an "empty" database password, i.e. no password, but
    // that is not really helpful, from a security standpoint. So there's no
    // exception for that here.
    std::cerr << "Error: Empty values are not allowed in configuration file "
              << fileName << "!" << std::endl;
    return false;
  }

  if ((name == "task.directory") || (name == "tasks.directory"))
  {
    if (!tasksDirectory.empty())
    {
      std::cerr << "Error: Tasks directory is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    tasksDirectory = value;
  } // if tasks.directory
  else if ((name == "task.extension") || (name == "tasks.extension"))
  {
    if (!tasksExtension.empty())
    {
      std::cerr << "Error: Task file extension is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    tasksExtension = value;
  } // if tasks.extension
  else if ((name == "db.host") || (name == "database.host"))
  {
    if (!connInfo.hostname().empty())
    {
      std::cerr << "Error: Database host name is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    connInfo.setHostname(value);
  } // if db.host
  else if ((name == "db.name") || (name == "database.name"))
  {
    if (!connInfo.db().empty())
    {
      std::cerr << "Error: Database name is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    connInfo.setDb(value);
  } // if db.name
  else if ((name == "db.user") || (name == "database.user"))
  {
    if (!connInfo.user().empty())
    {
      std::cerr << "Error: Database user is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    connInfo.setUser(value);
  } // if db.user
  else if ((name == "db.password") || (name == "database.password"))
  {
    if (!connInfo.password().empty())
    {
      std::cerr << "Error: Database password is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    connInfo.setPassword(value);
  } // if db.password
  else if ((name == "db.port") || (name == "database.port"))
  {
    if (connInfo.port() != 0)
    {
      std::cerr << "Error: Database port is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    int port = -1;
    if (!stringToInt(value, port))
    {
      std::cerr << "Error: Port number in file " << fileName << " must be a "
                << " positive integer!" << std::endl;
      return false;
    }
    if ((port <= 0) || (port > 65535))
    {
      std::cerr << "Error: Port number in file " << fileName << " must be "
                << " within the range [1;65535]!" << std::endl;
      return false;
    }
    connInfo.setPort(static_cast<uint16_t>(port));
  } // if db.port
  else if ((name == "key.OpenWeatherMap") || (name == "key.owm") || (name == "key.openweathermap"))
  {
    if (!key(ApiType::OpenWeatherMap).empty())
    {
      std::cerr << "Error: API key for OpenWeatherMap is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    apiKeys[ApiType::OpenWeatherMap] = value;
  } // if key.owm
  else if ((name == "key.DarkSky") || (name == "key.darksky") || (name == "key.darkskynet"))
  {
    if (!key(ApiType::DarkSky).empty())
    {
      std::cerr << "Error: API key for DarkSky is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    apiKeys[ApiType::DarkSky] = value;
  } // if key.darksky
  else if ((name == "key.Weatherbit") || (name == "key.weatherbit"))
  {
    if (!key(ApiType::Weatherbit).empty())
    {
      std::cerr << "Error: API key for Weatherbit is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    apiKeys[ApiType::Weatherbit] = value;
  } // if key.weatherbit
  else if ((name == "key.weatherstack") || (name == "key.Weatherstack"))
  {
    if (!key(ApiType::Weatherstack).empty())
    {
      std::cerr << "Error: API key for Weatherstack is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    apiKeys[ApiType::Weatherstack] = value;
  } // if key.weatherstack
  else if ((name == "key.apixu") || (name == "key.Apixu"))
  {
    if (!key(ApiType::Apixu).empty())
    {
      std::cerr << "Error: API key for Apixu is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    apiKeys[ApiType::Apixu] = value;
  } // if key.apixu
  else if ((name == "plan.OpenWeatherMap") || (name == "plan.owm") || (name == "plan.openweathermap"))
  {
    if (planOpenWeatherMap() != PlanOwm::none)
    {
      std::cerr << "Error: Plan for OpenWeatherMap is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    const auto plan = toPlanOwm(value);
    if (plan == PlanOwm::none)
    {
      std::cerr << "Error: \"" << value << "\" in file " << fileName
                << " is not a recognized plan for OpenWeatherMap!" << std::endl;
      std::cerr << "Hint: Recognized OpenWeatherMap plans are:" << std::endl
                << "\t" << toString(PlanOwm::Free) << std::endl
                << "\t" << toString(PlanOwm::Startup) << std::endl
                << "\t" << toString(PlanOwm::Developer) << std::endl
                << "\t" << toString(PlanOwm::Professional) << std::endl
                << "\t" << toString(PlanOwm::Expert) << std::endl
                << "\t" << toString(PlanOwm::Enterprise) << std::endl;
      return false;
    }
    planOwm = plan;
  } // if plan.owm
  else if ((name == "plan.weatherbit") || (name == "plan.Weatherbit"))
  {
    if (planWeatherbit() != PlanWeatherbit::none)
    {
      std::cerr << "Error: Plan for Weatherbit is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    const auto plan = toPlanWeatherbit(value);
    if (plan == PlanWeatherbit::none)
    {
      std::cerr << "Error: \"" << value << "\" in file " << fileName
                << " is not a recognized plan for Weatherbit!" << std::endl;
      std::cerr << "Hint: Recognized Weatherbit plans are:" << std::endl
                << "\t" << toString(PlanWeatherbit::Free) << std::endl
                << "\t" << toString(PlanWeatherbit::Standard) << std::endl
                << "\t" << toString(PlanWeatherbit::Plus) << std::endl
                << "\t" << toString(PlanWeatherbit::Business) << std::endl;
      return false;
    }
    planWb = plan;
  } // if plan.weatherbit
  else if ((name == "plan.weatherstack") || (name == "plan.Weatherstack"))
  {
    if (planWeatherstack() != PlanWeatherstack::none)
    {
      std::cerr << "Error: Plan for Weatherstack is specified more than once in file "
                << fileName << "!" << std::endl;
      return false;
    }
    const auto plan = toPlanWeatherstack(value);
    if (plan == PlanWeatherstack::none)
    {
      std::cerr << "Error: \"" << value << "\" in file " << fileName
                << " is not a recognized plan for Weatherstack!" << std::endl;
      std::cerr << "Hint: Recognized Weatherstack plans are:" << std::endl
                << "\t" << toString(PlanWeatherstack::Free) << std::endl
                << "\t" << toString(PlanWeatherstack::Standard) << std::endl
                << "\t" << toString(PlanWeatherstack::Professional) << std::endl
                << "\t" << toString(PlanWeatherstack::Business) << std::endl;
      return false;
    }
    planWs = plan;
  } // if plan.weatherstack
  else
  {
    std::cerr << "Error while reading configuration file " << fileName
              << ": There is no setting named \"" << name << "\"!" << std::endl;
    return false;
  } // else (unrecognized setting name)

  // Success.
  return true;
}

bool Configuration::loadCoreConfiguration(const std::string& fileName)
{
  std::ifstream stream(fileName, std::ios_base::in | std::ios_base::binary);
  if (!stream.good())
  {
    std::cerr << "Error: Could not open configuration file " << fileName << "!" << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(stream, line))
  {
    trim(line);
    // skip empty lines and comment lines
    if (line.empty() || line[0] == commentCharacter)
      continue;

    // check for possible carriage return at end (happens on Windows systems)
    if (line.at(line.length() - 1) == '\r')
    {
      line.erase(line.length() - 1);
    } // if

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

    if (!loadCoreConfigurationValue(name, value, fileName))
      return false;
  } // while

  // Set database port to default for MariaDB, if it has not been set yet.
  if (connInfo.port() == 0)
  {
    connInfo.setPort(ConnectionInformation::defaultMariaDbPort);
  }
  if (!connInfo.isComplete())
  {
    std::cerr << "Error: Database connection setting in file " << fileName
              << " are incomplete! Specify hostname, database name, user, "
              << "password and (if not " << ConnectionInformation::defaultMariaDbPort
              << ") the port number!" << std::endl;
    return false;
  }

  // For backwards compatibility with configuration files from version 0.9.8 and
  // earlier the unset plans are assumed to be free plans.
  if (planOpenWeatherMap() == PlanOwm::none)
    planOwm = PlanOwm::Free;
  if (planWeatherbit() == PlanWeatherbit::none)
    planWb = PlanWeatherbit::Free;
  if (planWeatherstack() == PlanWeatherstack::none)
    planWs = PlanWeatherstack::Free;

  // Everything is good, so far.
  return true;
}

#ifndef wic_no_tasks_in_config
bool Configuration::load(const std::string& fileName, const bool skipTasks, const bool missingKeysAllowed)
#else
bool Configuration::load(const std::string& fileName, [[maybe_unused]] const bool skipTasks, [[maybe_unused]] const bool missingKeysAllowed)
#endif
{
  namespace fs = std::filesystem;

  std::string realName(fileName);
  findConfigurationFile(realName);
  if (realName.empty())
  {
    std::cerr << "Error: No configuration file was found!" << std::endl;
    return false;
  }
  try
  {
    fs::path p(realName);
    if (!fs::exists(p) || !fs::is_regular_file(p))
    {
      std::cerr << "Error: Configuration file " << realName
                << " does not exist or is not a regular file!" << std::endl;
      return false;
    }
  }
  catch (const fs::filesystem_error& ex)
  {
    std::cerr << "File system error while checking existence of file "
              << realName << ": " << ex.what() << std::endl;
  } // try-catch

  // clear any existing information
  clear();

  // load core configuration file
  if (!loadCoreConfiguration(realName))
    return false;

  #ifndef wic_no_tasks_in_config
  tasksContainer.clear();
  // If we do not want task data, exit here.
  if (skipTasks)
  {
    return true;
  }

  findTaskDirectory(tasksDirectory);
  if (tasksDirectory.empty())
  {
    std::cerr << "Error: No task directory was found!" << std::endl;
    return false;
  } // if
  try
  {
    fs::path p(taskDirectory());
    if (!fs::exists(p) || !fs::is_directory(p))
    {
      std::cerr << "Error: Task directory " << taskDirectory()
                << " does not exist or is not a directory!" << std::endl;
      return false;
    }
  }
  catch (const fs::filesystem_error& ex)
  {
    std::cerr << "File system error while checking existence of directory "
              << taskDirectory() << ": " << ex.what() << std::endl;
  } // try-catch

  if (!TaskManager::loadFromDirectory(taskDirectory(), taskExtension(), tasksContainer, planWb, planWs))
    return false;
  // check for duplicates
  if (TaskManager::hasDuplicates(tasksContainer, false))
    return false;

  // Warn, if task container is empty.
  if (tasksContainer.empty())
  {
    std::clog << "Warning: Task list is empty!" << std::endl;
  }
  // Quit, if there are Apixu tasks, because Apixu API has been deprecated.
  const auto apixuTaskCount = std::count_if(tasksContainer.begin(), tasksContainer.end(),
                                  [](const Task& t) { return t.api() == ApiType::Apixu; });
  if (apixuTaskCount > 0)
  {
    if (apixuTaskCount > 1)
      std::cerr << "Error: There are " << apixuTaskCount << " tasks configured that use the Apixu API.";
    else
      std::cerr << "Error: There is one task configured that uses the Apixu API.";
    std::cerr << std::endl << "However, the Apixu API has been deprecated and shut down." << std::endl
              << "Please remove the Apixu task file(s) or consider switching "
              << "to another API, e.g. Open-Meteo, OpenWeatherMap, Weatherbit or Weatherstack." << std::endl;
    return false;
  }
  // Quit, if there are DarkSky tasks, because Dark Sky API has been shut down
  // on 31st March 2023.
  const auto darkSkyTaskCount = std::count_if(tasksContainer.begin(), tasksContainer.end(),
                                  [](const Task& t) { return t.api() == ApiType::DarkSky; });
  if (darkSkyTaskCount > 0)
  {
    if (darkSkyTaskCount > 1)
      std::cerr << "Error: There are " << darkSkyTaskCount << " tasks configured that use the DarkSky API.";
    else
      std::clog << "Error: There is one task configured that uses the DarkSky API.";
    std::clog << std::endl << "However, the DarkSky API was shut down on 31st March 2023." << std::endl
              << "Please remove the DarkSky task file(s) or consider switching "
              << "to another API, e.g. Open-Meteo, OpenWeatherMap, Weatherbit or Weatherstack." << std::endl;
    return false;
  }
  // Check whether there are tasks that need an API key but where the key for
  // that API is missing.
  if (!missingKeysAllowed)
  {
    for (const auto& t: tasksContainer)
    {
      if (needsApiKey(t.api()) && key(t.api()).empty())
      {
        std::cerr << "Error: There is at least one task using the "
                  << toString(t.api()) << " API, but the configuration file "
                  << fileName << " does not contain an API key for that API."
                  << std::endl;
        return false;
      }
    }
  }
  #endif // wic_no_tasks_in_config

  return true;
}

void Configuration::clear()
{
  // clear information
  #ifndef wic_no_tasks_in_config
  tasksContainer.clear();
  #endif // wic_no_tasks_in_config
  apiKeys.clear();
  connInfo.clear();
  tasksDirectory.erase();
  tasksExtension.erase();
}

} // namespace
