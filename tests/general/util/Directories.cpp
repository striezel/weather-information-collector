/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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

#include <catch.hpp>
#include "../../../src/util/Directories.hpp"

TEST_CASE("directory functions")
{
  SECTION("getHome")
  {
    std::string home;
    REQUIRE( wic::filesystem::getHome(home) );
    REQUIRE_FALSE( home.empty() );
    // Check some common locations.
    const bool startsWithCUsers = (home.find("C:\\Users\\") == 0)
                               || (home.find("c:\\Users\\") == 0)
                               || (home.find("c:\\users\\") == 0);
    const bool windowsSystemProfile = (home.find("C:\\Windows\\System32\\config\\systemprofile") == 0);
    const bool startsWithHome = (home.find("/home/") == 0);
    const bool startsWithRoot = (home.find("/root") == 0);
    const bool isCommon =  startsWithCUsers || windowsSystemProfile || startsWithHome || startsWithRoot;
    REQUIRE( isCommon );
  }
}
