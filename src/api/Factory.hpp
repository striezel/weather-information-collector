/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018, 2020  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_FACTORY_HPP
#define WEATHER_INFORMATION_COLLECTOR_FACTORY_HPP

#include <memory>
#include "API.hpp"
#include "Plans.hpp"

namespace wic
{

/** \brief "Factory" for API instances.
 */
class Factory
{
  public:
    /// no constructor needed
    Factory() = delete;

    /** \brief Creates an API instance based on the given API type.
     *
     * \param api     type of the API instance to create
     * \param planWs  current pricing plan for Weatherstack
     * \param key     the API key for that instance (optional)
     * \return Returns a unique_ptr to the created instance.
     *         Returns nullptr, if api is ApiType::none or not supported.
     */
    static std::unique_ptr<API> create(const ApiType api, const PlanWeatherstack planWs, const std::string& key = std::string());
}; // class

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_FACTORY_HPP
