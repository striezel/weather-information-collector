/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
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

#ifndef WEATHER_INFORMATION_COLLECTOR_TESTS_CIDATA_HPP
#define WEATHER_INFORMATION_COLLECTOR_TESTS_CIDATA_HPP

#include "../../src/data/Forecast.hpp"
#include "../../src/data/Location.hpp"
#include "../../src/data/Weather.hpp"

namespace wic
{

/** \brief Helper class to provide some data for CI runs. */
class CiData
{
  public:
    static Location getDresden();
    static Location getTiksi();
    static Forecast sampleForecastDresden();
    static Weather sampleWeatherDresden();


    /** \brief Generates a sequence of n Weather elements.
     *
     * \param start  basic element that will be used as start data point
     * \param n      number of elements to generate
     * \param diff   time difference between each subsequent element
     * \return Returns a vector containing n elements.
     */
    static std::vector<Weather> generate(Weather start, const std::size_t n = 10, const std::chrono::seconds diff = std::chrono::seconds(3600));
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_TESTS_CIDATA_HPP
