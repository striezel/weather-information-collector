/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector simdjson build test.
    Copyright (C) 2020  Dirk Stolle

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
#ifdef __SIZEOF_INT128__
#include "../../third-party/simdjson/simdjson.h"
#endif

int main(int argc, char** argv)
{
  simdjson::dom::parser parser;
  simdjson::dom::element elem;
  simdjson::error_code error;
  parser.load("does-not-exist.json").tie(elem, error);
  if (error)
  {
    std::cout << "Error parsing JSON file, but that was expected." << std::endl;
  }
  else
  {
    std::cout << "Parsing of JSON succeeded." << std::endl;
  }
  return 0;
}
