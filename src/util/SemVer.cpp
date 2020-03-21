/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019  Dirk Stolle

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

#include "SemVer.hpp"

namespace wic
{

SemVer::SemVer()
: majorVer(0), minorVer(0), patchVer(0)
{
}

SemVer::SemVer(const uint16_t major, const uint16_t minor, const uint16_t patch)
: majorVer(major), minorVer(minor), patchVer(patch)
{
}

std::string SemVer::toString() const
{
  return std::to_string(majorVer) + "." + std::to_string(minorVer) + "." + std::to_string(patchVer);
}

bool SemVer::operator==(const SemVer& other) const
{
  return majorVer == other.majorVer && minorVer == other.minorVer
      && patchVer == other.patchVer;
}

bool SemVer::operator<(const SemVer& other) const
{
  return (majorVer < other.majorVer)
    || ((majorVer == other.majorVer) && (minorVer < other.minorVer))
    || ((majorVer == other.majorVer) && (minorVer == other.minorVer) && (patchVer < other.patchVer));
}

} // namespace
