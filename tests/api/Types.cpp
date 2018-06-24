/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
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

#include <catch.hpp>
#include "../../src/api/Types.hpp"

TEST_CASE("API types")
{
  SECTION("to API type: apixu")
  {
    REQUIRE( wic::toApiType("apixu") == wic::ApiType::Apixu );
    REQUIRE( wic::toApiType("Apixu") == wic::ApiType::Apixu );
    REQUIRE( wic::toApiType("APIXU") == wic::ApiType::Apixu );
    REQUIRE( wic::toApiType("\t  ApIxU  ") == wic::ApiType::Apixu );
  }

  SECTION("to API type: OpenWeatherMap")
  {
    REQUIRE( wic::toApiType("openweathermap") == wic::ApiType::OpenWeatherMap );
    REQUIRE( wic::toApiType("OpenWeatherMap") == wic::ApiType::OpenWeatherMap );
    REQUIRE( wic::toApiType("OPENWEATHERMAP") == wic::ApiType::OpenWeatherMap );
    REQUIRE( wic::toApiType("\t  OpEnWEaTHeRmAp \t\t  ") == wic::ApiType::OpenWeatherMap );
  }

  SECTION("to API type: DarkSky")
  {
    REQUIRE( wic::toApiType("darksky") == wic::ApiType::DarkSky );
    REQUIRE( wic::toApiType("DarkSky") == wic::ApiType::DarkSky );
    REQUIRE( wic::toApiType("DARKSKY") == wic::ApiType::DarkSky );
    REQUIRE( wic::toApiType("\t  DaRkSkY  ") == wic::ApiType::DarkSky );
  }

  SECTION("to API type: none")
  {
    REQUIRE( wic::toApiType("none") == wic::ApiType::none );
    REQUIRE( wic::toApiType("None") == wic::ApiType::none );
    REQUIRE( wic::toApiType("NONE") == wic::ApiType::none );
    REQUIRE( wic::toApiType("\t  NoNe  ") == wic::ApiType::none );
  }

  SECTION("to API type: invalid names")
  {
    REQUIRE( wic::toApiType("foobar") == wic::ApiType::none );
    REQUIRE( wic::toApiType("blablablahhhhh!") == wic::ApiType::none );
    REQUIRE( wic::toApiType("this is none of our supported apis") == wic::ApiType::none );
    REQUIRE( wic::toApiType("") == wic::ApiType::none );
  }

  SECTION("to string")
  {
    REQUIRE( wic::toString(wic::ApiType::Apixu) == "Apixu" );
    REQUIRE( wic::toString(wic::ApiType::OpenWeatherMap) == "OpenWeatherMap" );
    REQUIRE( wic::toString(wic::ApiType::DarkSky) == "DarkSky" );
    REQUIRE( wic::toString(wic::ApiType::none) == "none" );
  }

  SECTION("to string - to API type - roundtrip")
  {
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::Apixu)) == wic::ApiType::Apixu );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::OpenWeatherMap)) == wic::ApiType::OpenWeatherMap );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::DarkSky)) == wic::ApiType::DarkSky );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::none)) == wic::ApiType::none );
  }

}
