/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_FIELD_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_FIELD_HPP

#include <string>
#include <mariadb/mysql.h>

namespace wic::db::mariadb
{

/** \brief Enumeration type for common field types. */
enum class FieldType {unknown, Integer, Float, Double, DateTime, String, Blob};

/** \brief Translates a database field type into the related enum.
 *
 * \param e   field type as seen in database
 * \param
 * \return Returns the corresponding FieldType value, if a match was found.
 *         Returns unknown, if no match was found.
 */
FieldType typeFromDbEnum(const enum_field_types e);

/** \brief Translates a FieldType enumeration into a human-readable string.
 *
 * \param ft  the FieldType to translate
 * \return Returns a string denoting the type, e. g. "integer", "float", etc.
 */
std::string toString(const FieldType ft);

/** \brief Holds information about a database / query field. */
struct Field
{
  /** \brief Constructs a new Field.
   *
   * \param _type  type of the field
   * \param _name  name of the field
   */
  Field(const FieldType _type, const std::string& _name);

  FieldType type; /**< type of the field, if known */
  std::string name; /**< name of the field */
}; // struct

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_FIELD_HPP
