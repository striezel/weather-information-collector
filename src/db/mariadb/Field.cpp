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

#include "Field.hpp"

namespace wic
{

namespace db
{

namespace mariadb
{

FieldType typeFromDbEnum(const enum_field_types e)
{
  switch (e)
  {
    case FIELD_TYPE_TINY:
    case FIELD_TYPE_SHORT:
    case FIELD_TYPE_INT24:
    case FIELD_TYPE_LONG:
    case FIELD_TYPE_LONGLONG:
         return FieldType::Integer;
    case FIELD_TYPE_FLOAT:
         return FieldType::Float;
    case FIELD_TYPE_DOUBLE:
         return FieldType::Double;
    case FIELD_TYPE_DATETIME:
         return FieldType::DateTime;
    case FIELD_TYPE_STRING:
    case FIELD_TYPE_VAR_STRING:
         return FieldType::String;
    case FIELD_TYPE_TINY_BLOB:
    case FIELD_TYPE_BLOB:
    case FIELD_TYPE_MEDIUM_BLOB:
    case FIELD_TYPE_LONG_BLOB:
         return FieldType::Blob;
    default:
         return FieldType::unknown;
  }
}

std::string toString(const FieldType ft)
{
  switch (ft)
  {
    case FieldType::Integer:
         return "integer";
    case FieldType::Float:
         return "float";
    case FieldType::Double:
         return "double";
    case FieldType::DateTime:
         return "datetime";
    case FieldType::String:
         return "string";
    case FieldType::Blob:
         return "blob";
    case FieldType::unknown:
    default:
         return "unknown";
  }
}

Field::Field(const FieldType _type, const std::string& _name)
: type(_type),
  name(_name)
{
}

} // namespace

} // namespace

} // namespace
