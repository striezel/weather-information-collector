/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020, 2022, 2025  Dirk Stolle

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

#include "../../find_catch.hpp"
#include "../../../src/api/Plans.hpp"

TEST_CASE("OpenWeatherMap plan types")
{
  SECTION("to type: free")
  {
    REQUIRE( wic::toPlanOwm("free") == wic::PlanOwm::Free );
    REQUIRE( wic::toPlanOwm("Free") == wic::PlanOwm::Free );
    REQUIRE( wic::toPlanOwm("FREE") == wic::PlanOwm::Free );
    REQUIRE( wic::toPlanOwm("\t  FrEe  \t") == wic::PlanOwm::Free );
  }

  SECTION("to type: startup")
  {
    REQUIRE( wic::toPlanOwm("startup") == wic::PlanOwm::Startup );
    REQUIRE( wic::toPlanOwm("Startup") == wic::PlanOwm::Startup );
    REQUIRE( wic::toPlanOwm("STARTUP") == wic::PlanOwm::Startup );
    REQUIRE( wic::toPlanOwm("\t  StArTuP  \t") == wic::PlanOwm::Startup );
  }

  SECTION("to type: developer")
  {
    REQUIRE( wic::toPlanOwm("developer") == wic::PlanOwm::Developer );
    REQUIRE( wic::toPlanOwm("Developer") == wic::PlanOwm::Developer );
    REQUIRE( wic::toPlanOwm("DEVELOPER") == wic::PlanOwm::Developer );
    REQUIRE( wic::toPlanOwm("\t  DeVeLoPeR  \t") == wic::PlanOwm::Developer );
  }

  SECTION("to type: professional")
  {
    REQUIRE( wic::toPlanOwm("professional") == wic::PlanOwm::Professional );
    REQUIRE( wic::toPlanOwm("Professional") == wic::PlanOwm::Professional );
    REQUIRE( wic::toPlanOwm("PROFESSIONAL") == wic::PlanOwm::Professional );
    REQUIRE( wic::toPlanOwm("\t  PrOfEsSiOnAl  \t") == wic::PlanOwm::Professional );
  }

  SECTION("to type: enterprise")
  {
    REQUIRE( wic::toPlanOwm("enterprise") == wic::PlanOwm::Enterprise );
    REQUIRE( wic::toPlanOwm("Enterprise") == wic::PlanOwm::Enterprise );
    REQUIRE( wic::toPlanOwm("ENTERPRISE") == wic::PlanOwm::Enterprise );
    REQUIRE( wic::toPlanOwm("\t  EnTeRpRiSe  \t") == wic::PlanOwm::Enterprise );
  }

  SECTION("to type: none")
  {
    REQUIRE( wic::toPlanOwm("none") == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm("None") == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm("NONE") == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm("\t  NoNe  ") == wic::PlanOwm::none );
  }

  SECTION("to type: invalid stuff")
  {
    REQUIRE( wic::toPlanOwm("") == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm("  \t   ") == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm("foobar") == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm("This is not a valid plan.") == wic::PlanOwm::none );
  }

  SECTION("to string")
  {
    REQUIRE( wic::toString(wic::PlanOwm::none) == "none" );
    REQUIRE( wic::toString(wic::PlanOwm::Free) == "free" );
    REQUIRE( wic::toString(wic::PlanOwm::Startup) == "startup" );
    REQUIRE( wic::toString(wic::PlanOwm::Developer) == "developer" );
    REQUIRE( wic::toString(wic::PlanOwm::Professional) == "professional" );
    REQUIRE( wic::toString(wic::PlanOwm::Enterprise) == "enterprise" );
  }

  SECTION("to string - to plan - roundtrip")
  {
    REQUIRE( wic::toPlanOwm(wic::toString(wic::PlanOwm::none)) == wic::PlanOwm::none );
    REQUIRE( wic::toPlanOwm(wic::toString(wic::PlanOwm::Free)) == wic::PlanOwm::Free );
    REQUIRE( wic::toPlanOwm(wic::toString(wic::PlanOwm::Startup)) == wic::PlanOwm::Startup );
    REQUIRE( wic::toPlanOwm(wic::toString(wic::PlanOwm::Developer)) == wic::PlanOwm::Developer );
    REQUIRE( wic::toPlanOwm(wic::toString(wic::PlanOwm::Professional)) == wic::PlanOwm::Professional );
    REQUIRE( wic::toPlanOwm(wic::toString(wic::PlanOwm::Enterprise)) == wic::PlanOwm::Enterprise );
  }
}

TEST_CASE("Weatherbit plan types")
{
  SECTION("to type: free")
  {
    REQUIRE( wic::toPlanWeatherbit("free") == wic::PlanWeatherbit::Free );
    REQUIRE( wic::toPlanWeatherbit("Free") == wic::PlanWeatherbit::Free );
    REQUIRE( wic::toPlanWeatherbit("FREE") == wic::PlanWeatherbit::Free );
    REQUIRE( wic::toPlanWeatherbit("\t  FrEe  \t") == wic::PlanWeatherbit::Free );
  }

  SECTION("to type: starter")
  {
    REQUIRE( wic::toPlanWeatherbit("starter") == wic::PlanWeatherbit::Starter );
    REQUIRE( wic::toPlanWeatherbit("Starter") == wic::PlanWeatherbit::Starter );
    REQUIRE( wic::toPlanWeatherbit("STARTER") == wic::PlanWeatherbit::Starter );
    REQUIRE( wic::toPlanWeatherbit("\t  StArTeR  \t") == wic::PlanWeatherbit::Starter );
  }

  SECTION("to type: developer")
  {
    REQUIRE( wic::toPlanWeatherbit("developer") == wic::PlanWeatherbit::Developer );
    REQUIRE( wic::toPlanWeatherbit("Developer") == wic::PlanWeatherbit::Developer );
    REQUIRE( wic::toPlanWeatherbit("DEVELOPER") == wic::PlanWeatherbit::Developer );
    REQUIRE( wic::toPlanWeatherbit("\t  DeVeLoPeR  \t") == wic::PlanWeatherbit::Developer );
  }

  SECTION("to type: advanced")
  {
    REQUIRE( wic::toPlanWeatherbit("advanced") == wic::PlanWeatherbit::Advanced );
    REQUIRE( wic::toPlanWeatherbit("Advanced") == wic::PlanWeatherbit::Advanced );
    REQUIRE( wic::toPlanWeatherbit("ADVANCED") == wic::PlanWeatherbit::Advanced );
    REQUIRE( wic::toPlanWeatherbit("\t  AdVaNcEd  \t") == wic::PlanWeatherbit::Advanced );
  }

  SECTION("to type: none")
  {
    REQUIRE( wic::toPlanWeatherbit("none") == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit("None") == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit("NONE") == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit("\t  NoNe  \t") == wic::PlanWeatherbit::none );
  }

  SECTION("to type: invalid stuff")
  {
    REQUIRE( wic::toPlanWeatherbit("") == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit("  \t   ") == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit("foobar") == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit("This is not a valid plan.") == wic::PlanWeatherbit::none );
  }

  SECTION("to string")
  {
    REQUIRE( wic::toString(wic::PlanWeatherbit::none) == "none" );
    REQUIRE( wic::toString(wic::PlanWeatherbit::Free) == "free" );
    REQUIRE( wic::toString(wic::PlanWeatherbit::Starter) == "starter" );
    REQUIRE( wic::toString(wic::PlanWeatherbit::Developer) == "developer" );
    REQUIRE( wic::toString(wic::PlanWeatherbit::Advanced) == "advanced" );
  }

  SECTION("to string - to plan - roundtrip")
  {
    REQUIRE( wic::toPlanWeatherbit(wic::toString(wic::PlanWeatherbit::none)) == wic::PlanWeatherbit::none );
    REQUIRE( wic::toPlanWeatherbit(wic::toString(wic::PlanWeatherbit::Free)) == wic::PlanWeatherbit::Free );
    REQUIRE( wic::toPlanWeatherbit(wic::toString(wic::PlanWeatherbit::Starter)) == wic::PlanWeatherbit::Starter );
    REQUIRE( wic::toPlanWeatherbit(wic::toString(wic::PlanWeatherbit::Developer)) == wic::PlanWeatherbit::Developer );
    REQUIRE( wic::toPlanWeatherbit(wic::toString(wic::PlanWeatherbit::Advanced)) == wic::PlanWeatherbit::Advanced );
  }
}

TEST_CASE("Weatherstack plan types")
{
  SECTION("to type: free")
  {
    REQUIRE( wic::toPlanWeatherstack("free") == wic::PlanWeatherstack::Free );
    REQUIRE( wic::toPlanWeatherstack("Free") == wic::PlanWeatherstack::Free );
    REQUIRE( wic::toPlanWeatherstack("FREE") == wic::PlanWeatherstack::Free );
    REQUIRE( wic::toPlanWeatherstack("\t  FrEe  \t") == wic::PlanWeatherstack::Free );
  }

  SECTION("to type: standard")
  {
    REQUIRE( wic::toPlanWeatherstack("standard") == wic::PlanWeatherstack::Standard );
    REQUIRE( wic::toPlanWeatherstack("Standard") == wic::PlanWeatherstack::Standard );
    REQUIRE( wic::toPlanWeatherstack("STANDARD") == wic::PlanWeatherstack::Standard );
    REQUIRE( wic::toPlanWeatherstack("\t  StAnDaRd  \t") == wic::PlanWeatherstack::Standard );
  }

  SECTION("to type: professional")
  {
    REQUIRE( wic::toPlanWeatherstack("professional") == wic::PlanWeatherstack::Professional );
    REQUIRE( wic::toPlanWeatherstack("Professional") == wic::PlanWeatherstack::Professional );
    REQUIRE( wic::toPlanWeatherstack("PROFESSIONAL") == wic::PlanWeatherstack::Professional );
    REQUIRE( wic::toPlanWeatherstack("\t  PrOfEsSiOnAl  \t") == wic::PlanWeatherstack::Professional );
  }

  SECTION("to type: business")
  {
    REQUIRE( wic::toPlanWeatherstack("business") == wic::PlanWeatherstack::Business );
    REQUIRE( wic::toPlanWeatherstack("Business") == wic::PlanWeatherstack::Business );
    REQUIRE( wic::toPlanWeatherstack("BUSINESS") == wic::PlanWeatherstack::Business );
    REQUIRE( wic::toPlanWeatherstack("\t  BuSiNeSs  \t") == wic::PlanWeatherstack::Business );
  }

  SECTION("to type: none")
  {
    REQUIRE( wic::toPlanWeatherstack("none") == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack("None") == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack("NONE") == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack("\t  NoNe  \t") == wic::PlanWeatherstack::none );
  }

  SECTION("to type: invalid stuff")
  {
    REQUIRE( wic::toPlanWeatherstack("") == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack("  \t   ") == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack("foobar") == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack("This is not a valid plan.") == wic::PlanWeatherstack::none );
  }

  SECTION("to string")
  {
    REQUIRE( wic::toString(wic::PlanWeatherstack::none) == "none" );
    REQUIRE( wic::toString(wic::PlanWeatherstack::Free) == "free" );
    REQUIRE( wic::toString(wic::PlanWeatherstack::Standard) == "standard" );
    REQUIRE( wic::toString(wic::PlanWeatherstack::Professional) == "professional" );
    REQUIRE( wic::toString(wic::PlanWeatherstack::Business) == "business" );
  }

  SECTION("to string - to plan - roundtrip")
  {
    REQUIRE( wic::toPlanWeatherstack(wic::toString(wic::PlanWeatherstack::none)) == wic::PlanWeatherstack::none );
    REQUIRE( wic::toPlanWeatherstack(wic::toString(wic::PlanWeatherstack::Free)) == wic::PlanWeatherstack::Free );
    REQUIRE( wic::toPlanWeatherstack(wic::toString(wic::PlanWeatherstack::Standard)) == wic::PlanWeatherstack::Standard );
    REQUIRE( wic::toPlanWeatherstack(wic::toString(wic::PlanWeatherstack::Professional)) == wic::PlanWeatherstack::Professional );
    REQUIRE( wic::toPlanWeatherstack(wic::toString(wic::PlanWeatherstack::Business)) == wic::PlanWeatherstack::Business );
  }
}
