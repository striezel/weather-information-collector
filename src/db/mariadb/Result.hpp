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

#ifndef WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_RESULT_HPP
#define WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_RESULT_HPP

#include <string>
#include <vector>

#include "Connection.hpp"
#include "Field.hpp"
#include "Row.hpp"

namespace wic::db::mariadb
{

/** \brief Represents a result of an SQL query. */
class Result
{
  public:
    /** \brief Default constructor. Creates an empty result, not for use.
     */
    Result();

    /** \brief Determines whether the query succeeded and the result data is good
     *         and can be used.
     *
     * \return Returns true, if the result data can be used.
     *         Returns false otherwise. In that case the result data shall not
     *         be used, it may be incomplete or missing.
     */
    bool good() const;

    /** \brief Determines whether the result has some rows.
     *
     * \return Returns true, if the result contains at least one row.
     *         Returns false otherwise.
     */
    bool hasRows() const;

    /** \brief Gets the number of rows in the result.
     *
     * \return Returns the number of rows in the result.
     */
    unsigned int rowCount() const;

    /** \brief Gets the result rows, all of them.
     *
     * \return Returns the result's rows.
     */
    const std::vector<Row>& rows() const;

    /** \brief Gets the result row denoted by the given zero-based index.
     *
     * \param index   zero-based index of the row
     * \return Returns the row with the given index.
     * \throw  std::out_of_range    the row index is out of range
     */
    const Row& row(const std::size_t index) const;

    /** \brief Gets metadata of the fields in the result.
     *
     * \return Returns a vector of field metadata.
     */
    const std::vector<Field>& fields() const;

    /** \brief Gets the zero-based index of a named field / column.
     *
     * \param name  the name of the field (case-sensitive)
     * \return Returns the zero-based index.
     * \throw  std::invalid_argument   if no column with the given name exists
     */
    std::size_t fieldIndex(const std::string& name) const;

    friend class Connection;
  private:
    bool ok; /**< indicates whether the result is in a consistent state */
    std::vector<Field> fieldData; /**< information about fields in result */
    std::vector<Row> data; /**< rows of the result, i. e. the actual data */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_DB_MARIADB_RESULT_HPP
