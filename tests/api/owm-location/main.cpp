/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector test suite.
    Copyright (C) 2019, 2020  Dirk Stolle

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

#include <fstream>
#include <iostream>
#include "location.hpp"
#include "../../../src/json/NLohmannJsonOwm.hpp"
#include "../../../src/json/SimdJsonOwm.hpp"

int main(int argc, char** argv)
{
  using namespace wic;

  if ((argc < 2) || (argv[1] == nullptr))
  {
    std::cerr << "Error: No JSON file name was specified!\n";
    std::cerr << "One file is needed:\n"
              << " * JSON file for locations when searching for 'Berlin' (owm.location.json)\n" << std::endl;
    return 1;
  }

  // test for location data
  const std::string jsonFileName = std::string(argv[1]);

  std::string json;
  {
    std::ifstream jsonStream;
    jsonStream.open(jsonFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file could not be opened!\n";
      return 1;
    }

    std::getline(jsonStream, json, '\0');
    jsonStream.close();
  }

  std::cout << "Parsing with simdjson ..." << std::endl;
  int ret = parseLocations<SimdJsonOwm>(json);
  if (ret != 0)
    return ret;

  std::cout << "Parsing with nlohmann/json ..." << std::endl;
  return  parseLocations<NLohmannJsonOwm>(json);
}
