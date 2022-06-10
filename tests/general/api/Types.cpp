/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2018, 2019, 2022  Dirk Stolle

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
#include "../../../src/api/Types.hpp"

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

  SECTION("to API type: Weatherbit")
  {
    REQUIRE( wic::toApiType("weatherbit") == wic::ApiType::Weatherbit );
    REQUIRE( wic::toApiType("Weatherbit") == wic::ApiType::Weatherbit );
    REQUIRE( wic::toApiType("WEATHERBIT") == wic::ApiType::Weatherbit );
    REQUIRE( wic::toApiType("\t  WeAtHeRbIt  ") == wic::ApiType::Weatherbit );
  }

  SECTION("to API type: Weatherstack")
  {
    REQUIRE( wic::toApiType("weatherstack") == wic::ApiType::Weatherstack );
    REQUIRE( wic::toApiType("Weatherstack") == wic::ApiType::Weatherstack );
    REQUIRE( wic::toApiType("WEATHERSTACK") == wic::ApiType::Weatherstack );
    REQUIRE( wic::toApiType("\t  WeAtHeRsTaCk  ") == wic::ApiType::Weatherstack );
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
    REQUIRE( wic::toString(wic::ApiType::Weatherbit) == "Weatherbit" );
    REQUIRE( wic::toString(wic::ApiType::Weatherstack) == "Weatherstack" );
    REQUIRE( wic::toString(wic::ApiType::none) == "none" );
  }

  SECTION("to string - to API type - roundtrip")
  {
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::Apixu)) == wic::ApiType::Apixu );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::OpenWeatherMap)) == wic::ApiType::OpenWeatherMap );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::DarkSky)) == wic::ApiType::DarkSky );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::Weatherbit)) == wic::ApiType::Weatherbit );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::Weatherstack)) == wic::ApiType::Weatherstack );
    REQUIRE( wic::toApiType(wic::toString(wic::ApiType::none)) == wic::ApiType::none );
  }
}


TEST_CASE("Data types")
{
  SECTION("to data type: current")
  {
    REQUIRE( wic::toDataType("current") == wic::DataType::Current );
    REQUIRE( wic::toDataType("Current") == wic::DataType::Current );
    REQUIRE( wic::toDataType("CURRENT") == wic::DataType::Current );
    REQUIRE( wic::toDataType("\t  cUrReNt   ") == wic::DataType::Current );
  }

  SECTION("to data type: forecast")
  {
    REQUIRE( wic::toDataType("forecast") == wic::DataType::Forecast );
    REQUIRE( wic::toDataType("Forecast") == wic::DataType::Forecast );
    REQUIRE( wic::toDataType("FORECAST") == wic::DataType::Forecast );
    REQUIRE( wic::toDataType("\t  FoReCaSt \t\t  ") == wic::DataType::Forecast );
  }

  SECTION("to data type: current+forecast")
  {
    REQUIRE( wic::toDataType("current+forecast") == wic::DataType::CurrentAndForecast );
    REQUIRE( wic::toDataType("Current+Forecast") == wic::DataType::CurrentAndForecast );
    REQUIRE( wic::toDataType("CURRENT+FORECAST") == wic::DataType::CurrentAndForecast );
    REQUIRE( wic::toDataType("\t  CuRrEnT+FoReCaSt  ") == wic::DataType::CurrentAndForecast );
  }

  SECTION("to data type: none")
  {
    REQUIRE( wic::toDataType("none") == wic::DataType::none );
    REQUIRE( wic::toDataType("None") == wic::DataType::none );
    REQUIRE( wic::toDataType("NONE") == wic::DataType::none );
    REQUIRE( wic::toDataType("\t  NoNe  ") == wic::DataType::none );
  }

  SECTION("to data type: invalid names")
  {
    REQUIRE( wic::toDataType("foobar") == wic::DataType::none );
    REQUIRE( wic::toDataType("blablablahhhhh!") == wic::DataType::none );
    REQUIRE( wic::toDataType("this is none of our supported apis") == wic::DataType::none );
    REQUIRE( wic::toDataType("") == wic::DataType::none );
  }

  SECTION("to string")
  {
    REQUIRE( wic::toString(wic::DataType::Current) == "current" );
    REQUIRE( wic::toString(wic::DataType::Forecast) == "forecast" );
    REQUIRE( wic::toString(wic::DataType::CurrentAndForecast) == "current+forecast" );
    REQUIRE( wic::toString(wic::DataType::none) == "none" );
  }

  SECTION("to string - to data type - roundtrip")
  {
    REQUIRE( wic::toDataType(wic::toString(wic::DataType::Current)) == wic::DataType::Current );
    REQUIRE( wic::toDataType(wic::toString(wic::DataType::Forecast)) == wic::DataType::Forecast );
    REQUIRE( wic::toDataType(wic::toString(wic::DataType::CurrentAndForecast)) == wic::DataType::CurrentAndForecast );
    REQUIRE( wic::toDataType(wic::toString(wic::DataType::none)) == wic::DataType::none );
  }
}
