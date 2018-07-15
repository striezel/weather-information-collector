/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#ifndef wic_sync
#include "../tasks/TaskManager.hpp"
#endif // wic_sync
#include "../util/Strings.hpp"

namespace wic
{

// use same comment character as in task files: '#'
const char Configuration::commentCharater = '#';

Configuration::Configuration()
:
  #ifndef wic_sync
  tasksContainer(std::vector<Task>()),
  #endif // wic_sync
  apiKeys(std::map<ApiType, std::string>()),
  connInfo(ConnectionInformation("", "", "", "", 0)),
  tasksDirectory(""),
  tasksExtension("")
{
}

std::vector<std::string> Configuration::potentialFileNames()
{
  return {
      "/etc/weather-information-collector/weather-information-collector.conf",
      "/etc/weather-information-collector/wic.conf",
      "/etc/wic/wic.conf",
      "./weather-information-collector.conf",
      "./wic.conf"
  };
}

std::vector<std::string> Configuration::potentialTaskDirectories()
{
  return {
      "/etc/weather-information-collector/task.d",
      "/etc/wic/task.d",
      "./task.d"
  };
}

const ConnectionInformation& Configuration::connectionInfo() const
{
  return connInfo;
}

#ifndef wic_sync
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
#endif // wic_sync

std::string Configuration::key(const ApiType api) const
{
  const auto iter = apiKeys.find(api);
  if (iter != apiKeys.end())
    return iter->second;
  else
    return std::string();
}

void Configuration::findConfigurationFile(std::string& realName)
{
  namespace fs = boost::filesystem;

  if (realName.empty())
  {
    for(const auto& file : potentialFileNames())
    {
      const fs::path path(file);
      try
      {
        if (fs::exists(path) && fs::is_regular_file(path))
        {
          realName = file;
          break;
        }
      }
      catch (const fs::filesystem_error& ex)
      {
        std::cerr << "File system error while checking existence of file "
                  << file << ": " << ex.what() << std::endl;
      } // try-catch
    } // for
  } // if
}

void Configuration::findTaskDirectory(std::string& realName)
{
  namespace fs = boost::filesystem;

  if (realName.empty())
  {
    for(const auto& dir : potentialTaskDirectories())
    {
      const fs::path path(dir);
      try
      {
        if (fs::exists(path) && fs::is_directory(path))
        {
          realName = dir;
          break;
        }
      }
      catch (const fs::filesystem_error& ex)
      {
        std::cerr << "File system error while checking existence of directory "
                  << dir << ": " << ex.what() << std::endl;
      } // try-catch
    } // for
  } // if
}

bool Configuration::loadCoreConfiguration(const std::string& fileName, const bool missingKeysAllowed)
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
    if (line.empty() || line[0] == commentCharater)
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
    else if ((name == "db.user") || (name == "database.user") )
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
    else
    {
      std::cerr << "Error while reading configuration file " << fileName
                << ": There is no setting named \"" << name << "\"!" << std::endl;
      return false;
    } // else (unrecognized setting name)
  } // while

  // Set database port to default for MySQL, if it has not been set yet.
  if (connInfo.port() == 0)
  {
    connInfo.setPort(ConnectionInformation::defaultMySqlPort);
  }
  if (!connInfo.isComplete())
  {
    std::cerr << "Error: Database connection setting in file " << fileName
              << " are incomplete! Specify hostname, database name, user, "
              << "password and (if not " << ConnectionInformation::defaultMySqlPort
              << ") the port number!" << std::endl;
    return false;
  }

  // Only check key presence, if missing keys are not allowed.
  if (!missingKeysAllowed)
  {
    // If there are no API keys, then the collector won't be able to collect
    // information later on.
    if (apiKeys.empty())
    {
      std::cerr << "Error: There are no API keys in configuration file "
                << fileName << ", and thus the weather-information-collector "
                << "will not be able to work properly." << std::endl;
      return false;
    } // if keys are missing
  } // if keys must be present

  // Everything is good, so far.
  return true;
}

bool Configuration::load(const std::string& fileName, const bool skipTasks, const bool missingKeysAllowed)
{
  namespace fs = boost::filesystem;

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
  if (!loadCoreConfiguration(realName, missingKeysAllowed))
    return false;

  #ifndef wic_sync
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

  if (!TaskManager::loadFromDirectory(taskDirectory(), taskExtension(), tasksContainer))
    return false;
  // check for duplicates
  if (TaskManager::hasDuplicates(tasksContainer))
    return false;

  // Warn, if task container is empty.
  if (tasksContainer.empty())
  {
    std::clog << "Warning: Task list is empty!" << std::endl;
  }
  #endif // wic_sync

  return true;
}

void Configuration::clear()
{
  // clear information
  #ifndef wic_sync
  tasksContainer.clear();
  #endif // wic_sync
  apiKeys.clear();
  connInfo.clear();
  tasksDirectory.erase();
  tasksExtension.erase();
}

} // namespace
