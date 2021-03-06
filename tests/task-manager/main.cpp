/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2019  Dirk Stolle

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
#include "../../src/tasks/TaskManager.hpp"

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

  /* ************************ *
   * tests for loadFromFile() *
   * ************************ */

  if (!tm.loadFromFile(baseDirectory + "/conf.d/test-example-hammelburg.conf", task))
  {
    std::cerr << "Error: Could not load task from file test-example-hammelburg.conf!\n";
    return 1;
  }

  // check data
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
  if (task.location().owmId() != 123)
  {
    std::cerr << "Error: Wrong OpenWeatherMap location ID!\n";
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

  // load second, whitespace version
  wic::Task whiteSpaceTask;
  if (!tm.loadFromFile(baseDirectory + "/conf.d/test-example-hammelburg-whitespace.conf", whiteSpaceTask))
  {
    std::cerr << "Error: Could not load task from file test-example-hammelburg-whitespace.conf!\n";
    return 1;
  }
  // compare data
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

  // load third file with country code
  {
    wic::Task countryTask;
    if (!tm.loadFromFile(baseDirectory + "/test-example-hammelburg-country.conf", countryTask))
    {
      std::cerr << "Error: Could not load task from file test-example-hammelburg-country.conf!\n";
      return 1;
    }
    // compare data
    if (countryTask.api() != wic::ApiType::Weatherbit)
    {
      std::cerr << "Error: API values do not match!\n"
                << wic::toString(task.api()) << " != Weatherbit\n";
      return 1;
    }
    if (countryTask.location().countryCode() != "DE")
    {
      std::cerr << "Error: Country code does not match!\n"
                << countryTask.location().countryCode() << " != DE\n";
      return 1;
    }
    if (task.interval() != countryTask.interval())
    {
      std::cerr << "Error: Request intervals do not match!\n";
      return 1;
    }
    if (task.data() != wic::DataType::Current)
    {
      std::cerr << "Error: Requested data type does not match 'current'!\n";
      return 1;
    }
    // check location
    wic::Location expectedLocation(task.location());
    expectedLocation.setCountryCode("DE");
    if (!(expectedLocation == countryTask.location()))
    {
      std::cerr << "Error: Location of task with country code does not match the expected value!\n";
      return 1;
    }
  }


  /* ***************************** *
   * tests for loadFromDirectory() *
   * ***************************** */

  std::vector<wic::Task> tasks;
  if (!tm.loadFromDirectory(baseDirectory + "/conf.d", ".conf", tasks, wic::PlanWeatherbit::Free, wic::PlanWeatherstack::Free))
  {
    std::cerr << "Error: Could not load tasks from directory " << baseDirectory
              << "!\n";
    return 2;
  }
  if (tasks.size() != 2)
  {
    std::cerr << "Error: Expected number of tasks from directory " << baseDirectory
              << " to be two, but there were " << tasks.size() << " tasks!\n";
    return 2;
  }
  // compare data with prev. task
  if (task.api() != tasks[0].api())
  {
    std::cerr << "Error: API values do not match!\n"
              << wic::toString(task.api()) << " != " << wic::toString(tasks[0].api()) << "\n";
    return 2;
  }
  if (task.interval() != tasks[0].interval())
  {
    std::cerr << "Error: Request intervals do not match!\n";
    return 2;
  }
  if (!(task.location() == tasks[0].location()))
  {
    std::cerr << "Error: Locations do not match!\n";
    return 2;
  }

  /* ************************************************** *
   * tests for loadFromDirectory() with incomplete data *
   * ************************************************** */

  tasks.clear();
  const std::string incompleteDirectory = baseDirectory + "/conf.d-incomplete";
  if (tm.loadFromDirectory(incompleteDirectory, ".conf", tasks, wic::PlanWeatherbit::Free, wic::PlanWeatherstack::Free))
  {
    std::cerr << "Error: Could load tasks from directory " << incompleteDirectory
              << ", although configuration file there is incomplete!" << std::endl;
    return 3;
  }
  std::cout << "Info: Passed test for incomplete task data." << std::endl;

  /* ************************* *
   * tests for hasDuplicates() *
   * ************************* */

  tasks.clear();
  // no duplicates in empty vector
  if (tm.hasDuplicates(tasks, false))
  {
    std::cerr << "Error: There cannot be any duplicates in an empty vector!\n";
    return 4;
  }
  if (!tm.loadFromDirectory(baseDirectory + "/conf.d", ".conf", tasks, wic::PlanWeatherbit::Free, wic::PlanWeatherstack::Free))
  {
    std::cerr << "Error: Could not load tasks from directory " << baseDirectory
              << "!\n";
    return 4;
  }
  if (tasks.size() != 2)
  {
    std::cerr << "Error: Expected number of tasks from directory " << baseDirectory
              << " to be two, but there were " << tasks.size() << " tasks!\n";
    return 4;
  }
  // some tasks are identical, so there should be duplicates
  if (!tm.hasDuplicates(tasks, false))
  {
    std::cerr << "Error: Duplicate detection failed!\n";
    return 4;
  }
  std::cout << "Info: Passed test #1 for duplicate task data." << std::endl;

  /* ************************************************* *
   * tests for hasDuplicates() with different interval *
   * ************************************************* */

  tasks.clear();
  const std::string overlapDirectory = baseDirectory + "/conf.d-overlap";
  if (!tm.loadFromDirectory(overlapDirectory, ".conf", tasks, wic::PlanWeatherbit::Free, wic::PlanWeatherstack::Free))
  {
    std::cerr << "Error: Could not load tasks from directory " << overlapDirectory
              << "!\n";
    return 5;
  }
  if (tasks.size() != 2)
  {
    std::cerr << "Error: Expected number of tasks from directory " << overlapDirectory
              << " to be two, but there were " << tasks.size() << " tasks!\n";
    return 5;
  }
  // Both tasks are identical, except for interval, so there should be duplicates.
  if (!tm.hasDuplicates(tasks, false))
  {
    std::cerr << "Error: Duplicate detection failed!\n";
    return 5;
  }
  std::cout << "Info: Passed test #2 for duplicate task data." << std::endl;

  // All tests passed.
  return 0;
}
