/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2023, 2025  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTION_PLANS_HPP
#define WEATHER_INFORMATION_COLLECTION_PLANS_HPP

#include <string>

namespace wic
{

/** \brief enumeration type for plans of OpenWeatherMap
 */
enum class PlanOwm
{
    none,
    Free,
    Startup,
    Developer,
    Professional,
    Enterprise
};


/** \brief Gets the name of the OpenWeatherMap plan.
 *
 * \param type  the plan
 * \return Returns a string that identifies the plan.
 */
std::string toString(const PlanOwm type);


/** \brief Converts a name / string to a OpenWeatherMap plan.
 *
 * \param planName  name of the plan
 * \return Returns the enumeration value for the plan.
 *         Returns PlanOwm::none, if no match could be found.
 */
PlanOwm toPlanOwm(const std::string& planName);

/** \brief enumeration type for plans of Weatherbit
 */
enum class PlanWeatherbit
{
    none,
    Free,
    Standard,
    Plus,
    Business
};


/** \brief Gets the name of the Weatherbit plan.
 *
 * \param type  the plan
 * \return Returns a string that identifies the plan.
 */
std::string toString(const PlanWeatherbit type);


/** \brief Converts a name / string to a Weatherbit plan.
 *
 * \param planName  name of the plan
 * \return Returns the enumeration value for the plan.
 *         Returns PlanWeatherbit::none, if no match could be found.
 */
PlanWeatherbit toPlanWeatherbit(const std::string& planName);

/** \brief enumeration type for plans of Weatherstack
 */
enum class PlanWeatherstack
{
    none,
    Free,
    Standard,
    Professional,
    Business
};


/** \brief Gets the name of the Weatherstack plan.
 *
 * \param type  the plan
 * \return Returns a string that identifies the plan.
 */
std::string toString(const PlanWeatherstack type);


/** \brief Converts a name / string to a Weatherstack plan.
 *
 * \param planName  name of the plan
 * \return Returns the enumeration value for the plan.
 *         Returns PlanWeatherstack::none, if no match could be found.
 */
PlanWeatherstack toPlanWeatherstack(const std::string& planName);

} // namespace

#endif // WEATHER_INFORMATION_COLLECTION_PLANS_HPP
