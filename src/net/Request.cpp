/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2021  Dirk Stolle

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

#include "Request.hpp"
#include <iostream>
#include "Curly.hpp"

std::optional<std::string> Request::get(const std::string& url, const std::string_view fn)
{
  std::string response;
  Curly curly;
  curly.setURL(url);
  if (!curly.perform(response))
  {
    return {};
  }
  if (curly.getResponseCode() != 200)
  {
    std::cerr << "Error in " << fn << "(): Unexpected HTTP status code "
              << curly.getResponseCode() << "!" << std::endl;
    const auto & rh = curly.responseHeaders();
    std::cerr << "HTTP response headers (" << rh.size() << "):" << std::endl;
    for (const auto & s : rh)
    {
      std::cerr << "    " << s << std::endl;
    }
    return {};
  }
  return response;
}
