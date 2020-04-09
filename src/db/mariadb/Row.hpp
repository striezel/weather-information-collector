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


#ifndef WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_ROW_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_ROW_HPP

#include <chrono>
#include <string>
#include <vector>

namespace wic
{

namespace db
{

namespace mariadb
{

/** \brief Represents a row from a result of a database query. */
class Row
{
  public:
    /** \brief Represents a column's value. */
    struct Value
    {
      /** \brief Constructs a new value.
       *
       * \param _isNull  whether the field is NULL
       * \param _data    actual data of the field as string
       */
      Value(const bool _isNull, const std::string& _data);

      bool isNull; /**< whether the database value is NULL */
      std::string data; /**< actual data as string */
    }; // struct


    /** \brief Constructs an empty row.
     */
    Row();

    /** \brief Constructs a row from a vector of values.
     *
     * \param data   vector that will be used as row data
     */
    Row(std::vector<Value>&& _data);

    /** \brief Checks whether the value of the index-th column is NULL.
     *
     * \param index   zero-based index of the column
     * \return Returns true, if the value is NULL. Returns false otherwise.
     * \throw  std::out_of_range    the index is out of range
     */
    bool isNull(const std::size_t index) const;

    /** \brief Returns the value of the index-th column as string.
     *
     * \param index   zero-based index of the column
     * \return Returns value of the given column in the row.
     * \throw  std::out_of_range    the index is out of range
     */
    const std::string& column(const std::size_t index) const;

    /** \brief Returns the value of the index-th column as string.
     *
     * \param index   zero-based index of the column
     * \return Returns value of the given column in the row.
     *         If the index is out of range, behaviour is undefined.
     */
    const std::string& operator[](const std::size_t index) const;

    /** \brief Returns the value of the index-th column as integer.
     *
     * \param index   zero-based index of the column
     * \return Returns value of the given column in the row.
     * \throw  std::out_of_range    the index is out of range
     * \throw  std::out_of_range    value does not fit into range of int
     * \throw  std::invalid_argument value cannot be converted to int
     */
    int getInt(const std::size_t index) const;

    /** \brief Returns the value of the index-th column as float.
     *
     * \param index   zero-based index of the column
     * \return Returns value of the given column in the row.
     * \throw  std::out_of_range    the index is out of range
     * \throw  std::out_of_range    value does not fit into range of float
     * \throw  std::invalid_argument value cannot be converted to float
     */
    float getFloat(const std::size_t index) const;

    /** \brief Returns the value of the index-th column as double.
     *
     * \param index   zero-based index of the column
     * \return Returns value of the given column in the row.
     * \throw  std::out_of_range    the index is out of range
     * \throw  std::out_of_range    value does not fit into range of double
     * \throw  std::invalid_argument value cannot be converted to double
     */
    double getDouble(const std::size_t index) const;

    /** \brief Returns the value of the index-th column as time point.
     *
     * \param index   zero-based index of the column
     * \return Returns value of the given column in the row.
     * \throw  std::out_of_range    the index is out of range
     * \throw  std::invalid_argument value cannot be converted to date
     */
    std::chrono::time_point<std::chrono::system_clock> getDateTime(const std::size_t index) const;
  private:
    std::vector<Value> data; /**< data of the row as strings, one per columns */
}; // class

} // namespace

} // namespace

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_ROW_HPP
