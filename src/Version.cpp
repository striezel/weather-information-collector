/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018, 2019, 2020, 2021  Dirk Stolle

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

#include "Version.hpp"
#include <iostream>
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
#include "../third-party/simdjson/simdjson.h"
#else
#include "../third-party/nlohmann/json.hpp"
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing
#include "util/GitInfos.hpp"

namespace wic
{

void showVersion(const std::string_view name)
{
  GitInfos info;
  std::cout << name << ", " << wic::version << "\n"
            << "\n"
            << "Version control commit: " << info.commit() << "\n"
            << "Version control date:   " << info.date() << std::endl;
#ifndef wic_no_json_parsing
#ifdef __SIZEOF_INT128__
  std::cout << "JSON library:           simdjson " << SIMDJSON_STRINGIFY(SIMDJSON_VERSION) << "\n"
            << "                        using implementation "
            << simdjson::get_active_implementation()->name() << " ("
            << simdjson::get_active_implementation()->description() << ")" << std::endl;
#else
  std::cout << "JSON library:           nlohmann/json "
            << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR
            << "." << NLOHMANN_JSON_VERSION_PATCH << std::endl;
#endif // __SIZEOF_INT128__
#endif // wic_no_json_parsing

}

} // namespace
