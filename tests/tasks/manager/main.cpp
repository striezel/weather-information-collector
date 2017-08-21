/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
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

#include <iostream>
#include "../../../src/tasks/TaskManager.hpp"

int main(int argc, char** argv)
{
  if ((argc < 2) || (argv[1] == nullptr))
  {
    std::cerr << "Error: No Example files directory was specified!\n";
    return 1;
  }
  const std::string baseDirectory = std::string(argv[1]);

  wic::Task task;
  wic::TaskManager tm;

  if (!tm.loadFromFile(baseDirectory + "/test-example-hammelburg.conf", task))
  {
    std::cerr << "Error: Could not load task from file test-example-hammelburg.conf!\n";
    return 1;
  }

  //check data
  if (task.api() != wic::ApiType::OpenWeatherMap)
  {
    std::cerr << "Error: Unexpected API type!\n";
    return 1;
  }
  if (task.interval() != std::chrono::seconds(900))
  {
    std::cerr << "Error: Wrong interval!\n";
    return 1;
  }
  if (task.location().id() != 123)
  {
    std::cerr << "Error: Wrong location ID!\n";
    return 1;
  }
  if (task.location().name() != "Hammelburg")
  {
    std::cerr << "Error: Wrong location name!\n";
    return 1;
  }
  if (task.location().postcode() != "97762")
  {
    std::cerr << "Error: Wrong postcode!\n";
    return 1;
  }
  wic::Location l2;
  l2.setCoordinates(50.1, 9.8);
  if (!task.location().equalCoordinates(l2))
  {
    std::cerr << "Error: Wrong coordinates!\n";
    return 1;
  }

  //load second, whitespace version
  wic::Task whiteSpaceTask;
  if (!tm.loadFromFile(baseDirectory + "/test-example-hammelburg-whitespace.conf", whiteSpaceTask))
  {
    std::cerr << "Error: Could not load task from file test-example-hammelburg-whitespace.conf!\n";
    return 1;
  }
  //compare data
  if (task.api() != whiteSpaceTask.api())
  {
    std::cerr << "Error: API values do not match!\n"
              << wic::toString(task.api()) << " != " << wic::toString(whiteSpaceTask.api()) << "\n";
    return 1;
  }
  if (task.interval() != whiteSpaceTask.interval())
  {
    std::cerr << "Error: Request intervals do not match!\n";
    return 1;
  }
  if (!(task.location() == whiteSpaceTask.location()))
  {
    std::cerr << "Error: Locations do not match!\n";
    return 1;
  }

  //all tests passed
  return 0;
}
