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

#include "LocationWithCountry.hpp"

namespace wic
{

LocationWithCountry::LocationWithCountry()
: Location(), mCountry("")
{
}

const std::string& LocationWithCountry::country() const
{
  return mCountry;
}

void LocationWithCountry::setCountry(const std::string& newCountry)
{
  mCountry = newCountry;
}

bool LocationWithCountry::hasCountry() const
{
  return !mCountry.empty();
}

} // namespace

