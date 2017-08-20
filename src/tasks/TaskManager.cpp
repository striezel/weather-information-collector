/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017  Dirk Stolle

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
#include <fstream>
#include <iostream>
#include "../util/Strings.hpp"

namespace wic
{

const char TaskManager::commentCharater = '#';

bool TaskManager::loadFromFile(const std::string& fileName, Task& task)
{
  std::ifstream stream(fileName, std::ios_base::in | std::ios_base::binary);
  if (!stream.good())
  {
    std::cerr << "Error: Could not open file " << fileName << "!\n";
    return false;
  }
  task = Task();
  std::string line;
  while (std::getline(stream, line))
  {
    trim(line);
    //skip empty lines and comment lines
    if (line.empty() || line[0] == commentCharater)
      continue;

    //check for possible carriage return at end (happens on Windows systems)
    if (!line.empty())
    {
      if (line.at(line.length() - 1) == '\r')
      {
        line.erase(line.length() - 1);
      }//if
    }

    const auto sepPos = line.find('=');
    if (sepPos == std::string::npos)
    {
      std::cerr << "Error: Invalid line found: \"" << line <<"\".\n"
                << "General format: \"Name of Setting=value\"\n";
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
                << fileName << "!\n";
      return false;
    }
    //test possible values
    if (name == "api")
    {
      if (task.api() != ApiType::none)
      {
        std::cerr << "Error: API type is specified more than once in file "
                  << fileName << "!\n";
        return false;
      }
      const ApiType api = toApiType(value);
      if (api == ApiType::none)
      {
        std::cerr << "Error: API type \"" << value << "\" in file " << fileName
                  << " is not a recognized API!\n";
        return false;
      }
      task.setApi(api);
    } //if api
    else if ((name == "interval") || (name == "interval_seconds") || (name == "seconds"))
    {
      if (task.interval() > std::chrono::seconds::zero())
      {
        std::cerr << "Error: Request interval is specified more than once in file "
                  << fileName << "!\n";
        return false;
      }
      int seconds = -1;
      if (!stringToInt(value, seconds))
      {
        std::cerr << "Error: Request interval in file " << fileName
                  << " must be a positive integer value!\n";
        return false;
      }
      if (seconds < 15)
      {
        std::cerr << "Error: Minimum request interval in file " << fileName
                  << " must be 15 seconds!\n";
        return false;
      }
      task.setInterval(std::chrono::seconds(seconds));
    } //if interval
    else if ((name == "location.id") || (name == "location_id"))
    {
      if (task.location().hasId())
      {
        std::cerr << "Error: Location ID is specified more than once in file "
                  << fileName << "!\n";
        return false;
      }
      int id = -1;
      if (!stringToInt(value, id))
      {
        std::cerr << "Error: Location ID in file " << fileName
                  << " must be a positive integer value!\n";
        return false;
      }
      if (id < 1)
      {
        std::cerr << "Error: Location ID in file " << fileName
                  << " must be a positive integer value!\n";
        return false;
      }
      Location loc(task.location());
      loc.setId(id);
      task.setLocation(loc);
    } //if location.id
    else if ((name == "location.name") || (name == "location_name") || (name == "city"))
    {
      if (task.location().hasName())
      {
        std::cerr << "Error: Location name is specified more than once in file "
                  << fileName << "!\n";
        return false;
      }
      if (value.size() > 200)
      {
        std::cerr << "Error: Location name in file " << fileName
                  << " is unreasonably long!\n";
        return false;
      }
      Location loc(task.location());
      loc.setName(value);
      task.setLocation(loc);
    } //if location.name
    else if ((name == "location.postcode") || (name == "location_postcode") || (name == "postcode"))
    {
      if (task.location().hasPostcode())
      {
        std::cerr << "Error: Location's postcode is specified more than once in file "
                  << fileName << "!\n";
        return false;
      }
      if (value.size() > 25)
      {
        std::cerr << "Error: Location's postcode in file " << fileName
                  << " is unreasonably long!\n";
        return false;
      }
      Location loc(task.location());
      loc.setPostcode(value);
      task.setLocation(loc);
    } //if location.postcode
    else if ((name == "location.coordinates") || (name == "location_coordinates") || (name == "coordinates"))
    {
      if (task.location().hasLatitudeAndLongitude())
      {
        std::cerr << "Error: Location's coordinates are specified more than once in file "
                  << fileName << "!\n";
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
                  << " must be a floating point value!\n";
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
                  << " must be a floating point value!\n";
        return false;
      }
      if ((lon > 180.0f) || (lon < -1800.0f))
      {
        std::cerr << "Error: Longitude in file " << fileName
                  << " must be within the range [-180;+180]!\n";
        return false;
      }
      Location loc(task.location());
      loc.setLatitudeLongitude(lat, lon);
      task.setLocation(loc);
    } //if location.coordinates
  } //while
  //No more data, done.
  return true;
}

} //namespace

