/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2020, 2021, 2023  Dirk Stolle

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

#include "Plans.hpp"
#include "../util/Strings.hpp"

namespace wic
{

std::string toString(const PlanOwm type)
{
  switch (type)
  {
    case PlanOwm::Free:
         return "free";
    case PlanOwm::Startup:
         return "startup";
    case PlanOwm::Developer:
         return "developer";
    case PlanOwm::Professional:
         return "professional";
    case PlanOwm::Enterprise:
         return "enterprise";
    default: // i. e. PlanOwm::none
         return "none";
  }
}

PlanOwm toPlanOwm(const std::string& planName)
{
  auto name = toLowerString(planName);
  trim(name);
  if (name == "free")
    return PlanOwm::Free;
  if (name == "startup")
    return PlanOwm::Startup;
  if (name == "developer")
    return PlanOwm::Developer;
  if (name == "professional")
    return PlanOwm::Professional;
  if (name == "enterprise")
    return PlanOwm::Enterprise;
  // unknown / none
  return PlanOwm::none;
}

std::string toString(const PlanWeatherbit type)
{
  switch (type)
  {
    case PlanWeatherbit::Free:
         return "free";
    case PlanWeatherbit::Hobbyist:
         return "hobbyist";
    case PlanWeatherbit::Starter:
         return "starter";
    case PlanWeatherbit::Developer:
         return "developer";
    case PlanWeatherbit::Advanced:
         return "advanced";
    default: // i. e. PlanWeatherbit::none
         return "none";
  }
}

PlanWeatherbit toPlanWeatherbit(const std::string& planName)
{
  auto name = toLowerString(planName);
  trim(name);
  if (name == "free")
    return PlanWeatherbit::Free;
  if (name == "hobbyist")
    return PlanWeatherbit::Hobbyist;
  if (name == "starter")
    return PlanWeatherbit::Starter;
  if (name == "developer")
    return PlanWeatherbit::Developer;
  if (name == "advanced")
    return PlanWeatherbit::Advanced;
  // unknown / none
  return PlanWeatherbit::none;
}

std::string toString(const PlanWeatherstack type)
{
  switch (type)
  {
    case PlanWeatherstack::Free:
         return "free";
    case PlanWeatherstack::Standard:
         return "standard";
    case PlanWeatherstack::Professional:
         return "professional";
    case PlanWeatherstack::Business:
         return "business";
    default: // i. e. PlanWeatherstack::none
         return "none";
  }
}

PlanWeatherstack toPlanWeatherstack(const std::string& planName)
{
  auto name = toLowerString(planName);
  trim(name);
  if (name == "free")
    return PlanWeatherstack::Free;
  if (name == "standard")
    return PlanWeatherstack::Standard;
  if (name == "professional")
    return PlanWeatherstack::Professional;
  if (name == "business")
    return PlanWeatherstack::Business;
  // unknown / none
  return PlanWeatherstack::none;
}

} // namespace
