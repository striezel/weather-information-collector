/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_STORECSV_HPP
#define WEATHER_INFORMATION_COLLECTOR_STORECSV_HPP

#include "Store.hpp"
#include <fstream>

namespace wic
{

/** \brief Stores weather data as CSV. */
class StoreCSV: public Store
{
  public:
      /** \brief default separator character
       */
    static const char defaultSeparator;


    /** \brief Constructor: initializes a file for CSV storage.
     *
     * \param fileName  name of the CSV file
     * \param separator the separator character, defaults to ';'
     * \throw May throw an exception, if the file could not be opened or created.
     */
    StoreCSV(const std::string& fileName, const char separator = defaultSeparator);


    /** \brief Destructor.
     */
    virtual ~StoreCSV();


    /** \brief Saves a current weather entry for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param weather   weather information
     * \return Returns true, if the data was saved.
     *         Returns false, if an error occurred.
     */
    virtual bool saveCurrentWeather(const ApiType type, const Location& location, const Weather& weather);


    /** \brief Saves weather forecast data for a given location and API.
     *
     * \param type      API that was used to gather the information
     * \param location  location for the weather information
     * \param forecast  weather forecast information
     * \return Returns true, if the data was saved.
     *         Returns false, if an error occurred.
     */
    virtual bool saveForecast(const ApiType type, const Location& location, const Forecast& forecast);


    /** \brief Writes uncommitted changes / data to the underlying CSV file.
     *
     * \throw May throw std::ios_base::failure.
     */
    virtual void flush();


    /** \brief Gets the CSV header line.
     *
     * \return Returns a string that contains a suitable CSV header.
     */
    std::string header() const;
  private:
    std::ofstream stream; /**< stream for the CSV file */
    char separator; /**< separator character */
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_STORECSV_HPP
