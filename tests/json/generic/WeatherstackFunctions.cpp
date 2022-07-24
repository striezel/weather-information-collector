/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2022  Dirk Stolle

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
#include <limits>
#include "../../../src/json/WeatherstackFunctions.hpp"

TEST_CASE("WeatherstackFunctions")
{
  using namespace wic;

  SECTION("weatherstackPrecipitationDistinction")
  {
    Weather weather;

    SECTION("codes for rain")
    {
      const auto codes = {
          176, // Patchy rain nearby
          200, // Thundery outbreaks in nearby
          248, // Fog
          263, // Patchy light drizzle
          266, // Light drizzle
          293, // Patchy light rain
          296, // Light rain
          299, // Moderate rain at times
          302, // Moderate rain
          305, // Heavy rain at times
          308, // Heavy rain
          353, // Light rain shower
          356, // Moderate or heavy rain shower
          359, // Torrential rain shower
          386, // Patchy light rain in area with thunder
          389  // Moderate or heavy rain in area with thunder
      };
      for (const int code: codes)
      {
        weather.setRain(12.5f);
        weather.setSnow(12.5f);
        weatherstackPrecipitationDistinction(5.0f, weather, code);
        REQUIRE( weather.rain() == 5.0f );
        REQUIRE( weather.snow() == 0.0f );
      }
    }

    SECTION("codes for snow")
    {
      const auto codes = {
          179, // Patchy snow nearby
          227, // Blowing snow
          230, // Blizzard
          260, // Freezing fog
          323, // Patchy light snow
          326, // Light snow
          329, // Patchy moderate snow
          332, // Moderate snow
          335, // Patchy heavy snow
          338, // Heavy snow
          350, // Ice pellets
          368, // Light snow showers
          371, // Moderate or heavy snow showers
          374, // Light showers of ice pellets
          377, // Moderate or heavy showers of ice pellets
          392, // Patchy light snow in area with thunder
          395  // Moderate or heavy snow in area with thunder
      };
      for (const int code: codes)
      {
        weather.setRain(12.5f);
        weather.setSnow(12.5f);
        weatherstackPrecipitationDistinction(4.0f, weather, code);
        REQUIRE( weather.rain() == 0.0f );
        REQUIRE( weather.snow() == 4.0f );
      }
    }

    SECTION("codes for sleet")
    {
      const auto codes = {
          182, // Patchy sleet nearby
          317, // Light sleet
          320, // Moderate or heavy sleet
          362, // Light sleet showers
          365  // Moderate or heavy sleet showers
      };
      for (const int code: codes)
      {
        weather.setRain(12.5f);
        weather.setSnow(12.5f);
        weatherstackPrecipitationDistinction(3.0f, weather, code);
        REQUIRE( weather.rain() == 1.5f );
        REQUIRE( weather.snow() == 1.5f );
      }
    }

    SECTION("unknown code: temperature above zero")
    {
      weather.setTemperatureCelsius(12.5f);
      weatherstackPrecipitationDistinction(5.0f, weather, 42);
      REQUIRE( weather.rain() == 5.0f );
      REQUIRE( weather.snow() == 0.0f );
    }

    SECTION("unknown code: temperature below zero")
    {
      weather.setTemperatureCelsius(-2.5f);
      weatherstackPrecipitationDistinction(5.0f, weather, 42);
      REQUIRE( weather.rain() == 0.0f );
      REQUIRE( weather.snow() == 5.0f );
    }

    SECTION("unknown code: temperature is not set")
    {
      weather.setTemperatureCelsius(std::numeric_limits<float>::quiet_NaN());
      weatherstackPrecipitationDistinction(3.0f, weather, 42);
      REQUIRE( weather.rain() == 3.0f );
      REQUIRE( weather.snow() == 0.0f );
    }
  }
}
