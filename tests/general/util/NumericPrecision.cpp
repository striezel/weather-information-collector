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

#include <catch.hpp>
#include <utility>
#include <vector>
#include "../../../src/util/NumericPrecision.hpp"

TEST_CASE("NumericPrecision")
{
  using namespace wic;

  std::vector<std::pair<float, float> > numbers = {
    { -1.54999, -1.55 },
    { -0.440009f, -0.44f },
    { 3.36001, 3.36 },
    { 9.47865f, 9.479f },
    { 38.3f, 38.3f },
    { 42.602f, 42.602f },
    { 43.952f, 43.952f },
    { 275.123f, 275.123f },
    // old v0.8.1 rounding
    { 3.999f, 4.0f },
    { 4.001f, 4.0f }
  };

  SECTION("enforce")
  {
    for (const auto& pair: numbers)
    {
      REQUIRE( NumericPrecision<float>::enforce(pair.first) == pair.second );
    }
  }
}
