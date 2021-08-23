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

#include "curl_version.hpp"
#include <iostream>
#include "Curly.hpp"

void showCurlVersion()
{
  const auto ver = Curly::curlVersion();
  if (!ver.cURL.empty())
  {
    std::cout << "Network library:        curl " << ver.cURL;
    if (!ver.ssl.empty())
    {
      std::cout << " with " << ver.ssl;
      if (!ver.libz.empty())
        std::cout << " and zlib/" << ver.libz;
    }
    else if (!ver.libz.empty())
        std::cout << " with zlib/" << ver.libz;
    std::cout << std::endl;
  }
  else
  {
    std::cout << "Network library:        unknown version" << std::endl;
  }
}
