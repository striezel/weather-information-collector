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

#include "Strings.hpp"
#include <sstream>

namespace wic
{

std::string toLowerString(std::string str)
{
  const int diff = 'a'-'A';
  for (unsigned int i = 0; i < str.size(); ++i)
  {
    if (str[i] >= 'A' and str[i] <= 'Z')
    {
      str[i] = str[i] + diff;
    }
  }//for
  return str;
}

void trimLeft(std::string& str1)
{
  if (str1.empty()) return;
  //trim stuff at begin
  const int len = str1.length();
  int pos = 0;
  bool go_on = true;
  while (go_on)
  {
    const char ch = str1.at(pos);
    if ((ch == ' ') or (ch == '\t'))
    {
      ++pos;
      go_on = (pos<len);
    }
    else
    {
      go_on = false;
    }
  }//while
  if (pos >= len)
  {
    str1.clear();
    return;
  }
  else if (pos > 0)
  {
    str1.erase(0, pos);
  }
  return;
}

void trimRight(std::string& str1)
{
  if (str1.empty()) return;
  //trim stuff at end
  const int len = str1.length();
  int pos = len - 1;
  bool go_on = true;
  while (go_on)
  {
    const char ch = str1.at(pos);
    if ((ch == ' ') or (ch == '\t'))
    {
      --pos;
      go_on = (pos >= 0);
    }
    else
    {
      go_on = false;
    }
  }//while
  if (pos == -1)
  {
    str1.clear();
  }
  else if (pos < len - 1)
  {
    str1.erase(pos + 1);
  }
  return;
}

void trim(std::string& str1)
{
  trimLeft(str1);
  trimRight(str1);
  return;
} //end of trim

std::string intToString(const int value)
{
  std::ostringstream s;
  s << value;
  return s.str();
}

std::string floatToString(const float f)
{
  std::stringstream s_str;
  s_str << f;
  return s_str.str();
}

} //namespace
