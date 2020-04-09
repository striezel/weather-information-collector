/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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

#include "Result.hpp"
#include <algorithm>
#include <stdexcept>

namespace wic
{

namespace db
{

namespace mariadb
{

Result::Result()
: ok(false),
  fieldData(std::vector<Field>()),
  data(std::vector<Row>())
{
}

bool Result::good() const
{
  return ok;
}

bool Result::hasRows() const
{
  return !data.empty();
}

unsigned int Result::rowCount() const
{
  return data.size();
}

const std::vector<Row>& Result::rows() const
{
  return data;
}

const Row& Result::row(const std::size_t index) const
{
  return data.at(index);
}

const std::vector<Field>& Result::fields() const
{
  return fieldData;
}

std::size_t Result::fieldIndex(const std::string& name) const
{
  const auto iter = std::find_if(fieldData.begin(), fieldData.end(), [&name](const Field& f){ return f.name == name; });
  if (iter != fieldData.end())
  {
    return iter - fieldData.begin();
  }
  throw std::invalid_argument("The result has no field named '" + name + "'.");
}

} // namespace

} // namespace

} // namespace
