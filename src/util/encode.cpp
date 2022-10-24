/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2021, 2022  Dirk Stolle

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

#include "encode.hpp"
#include <array>

namespace wic
{

std::string urlEncode(const std::string& str)
{
  static const std::array<char, 16> hexDigits = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  std::string encoded;
  for (const char c: str)
  {
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z') || (c == '-') || (c == '_')
      || (c == '.') || (c == '~'))
    {
      encoded.push_back(c);
    }
    else
    {
      // d
      encoded.push_back('%');
      const unsigned char code = static_cast<unsigned char>(c);
      encoded.push_back(hexDigits[code / 16]);
      encoded.push_back(hexDigits[code % 16]);
    }
  } // for
  return encoded;
}

} // namespace
