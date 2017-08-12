/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017  Dirk Stolle

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

#include "../../src/data/Weather.hpp"

TEST_CASE("WeatherClass")
{
  using namespace wic;
  wic::Weather weather;

  SECTION("empty after construction")
  {
    //no data should be set
    REQUIRE_FALSE( weather.hasDataTime() );
    REQUIRE_FALSE( weather.hasRequestTime() );
    REQUIRE_FALSE( weather.hasTemperatureKelvin() );
    REQUIRE_FALSE( weather.hasTemperatureCelsius() );
    REQUIRE_FALSE( weather.hasTemperatureFahrenheit() );
    REQUIRE_FALSE( weather.hasHumidity() );
    REQUIRE_FALSE( weather.hasWindSpeed() );
    REQUIRE_FALSE( weather.hasWindDegrees() );
    REQUIRE_FALSE( weather.hasCloudiness() );
    REQUIRE_FALSE( weather.hasJson() );
  }

  SECTION("set, has + get data time")
  {
    const auto dt = std::chrono::system_clock::now();
    weather.setDataTime(dt);
    REQUIRE( weather.hasDataTime() );
    REQUIRE( weather.dataTime() == dt );
  }

  SECTION("set, has + get request time")
  {
    const auto rt = std::chrono::system_clock::now() + std::chrono::seconds(5);
    weather.setRequestTime(rt);
    REQUIRE( weather.hasRequestTime() );
    REQUIRE( weather.requestTime() == rt );
  }

  SECTION("set, has + get temperature in Kelvin")
  {
    weather.setTemperatureKelvin(125.25);
    REQUIRE( weather.hasTemperatureKelvin() );
    REQUIRE( weather.temperatureKelvin() == 125.25 );
  }

  SECTION("set, has + get temperature in °C")
  {
    weather.setTemperatureCelsius(25.25);
    REQUIRE( weather.hasTemperatureCelsius() );
    REQUIRE( weather.temperatureCelsius() == 25.25 );
  }

  SECTION("set, has + get temperature in °F")
  {
    weather.setTemperatureFahrenheit(225.125);
    REQUIRE( weather.hasTemperatureFahrenheit() );
    REQUIRE( weather.temperatureFahrenheit() == 225.125 );
  }

  SECTION("set, has + get relative humidity")
  {
    weather.setHumidity(65);
    REQUIRE( weather.hasHumidity() );
    REQUIRE( weather.humidity() == 65 );
  }

  SECTION("out of range humidity")
  {
    weather.setHumidity(-5);
    REQUIRE_FALSE( weather.hasHumidity() );
    REQUIRE( weather.humidity() == -1 );

    weather.setHumidity(101);
    REQUIRE_FALSE( weather.hasHumidity() );
    REQUIRE( weather.humidity() == -1 );
  }

  SECTION("set, has + get wind speed")
  {
    weather.setWindSpeed(10.0f);
    REQUIRE( weather.hasWindSpeed() );
    REQUIRE( weather.windSpeed() == 10.0f );
  }

  SECTION("out of range wind speed")
  {
    weather.setWindSpeed(-10.0f);
    REQUIRE_FALSE( weather.hasWindSpeed() );
    const float ws = weather.windSpeed();
    REQUIRE( ws != ws );
  }

  SECTION("set, has + get wind direction")
  {
    weather.setWindDegrees(270);
    REQUIRE( weather.hasWindDegrees() );
    REQUIRE( weather.windDegrees() == 270 );
  }

  SECTION("invalid wind direction does not count")
  {
    weather.setWindDegrees(4445);
    REQUIRE_FALSE( weather.hasWindDegrees() );
    const auto degrees = weather.windDegrees();
    REQUIRE( degrees == -1 );

    weather.setWindDegrees(-180);
    REQUIRE_FALSE( weather.hasWindDegrees() );
    const auto deg2 = weather.windDegrees();
    REQUIRE( deg2 == -1 );
  }

  SECTION("set, has + get cloudiness")
  {
    weather.setCloudiness(85);
    REQUIRE( weather.hasCloudiness() );
    REQUIRE( weather.cloudiness() == 85 );
  }

  SECTION("invalid cloudiness values does not count")
  {
    weather.setCloudiness(101);
    REQUIRE_FALSE( weather.hasCloudiness() );
    REQUIRE( weather.cloudiness() == -1 );

    weather.setCloudiness(-5);
    REQUIRE_FALSE( weather.hasCloudiness() );
    REQUIRE( weather.cloudiness() == -1 );
  }

  SECTION("set, has + get JSON")
  {
    weather.setJson("{foo: 1, bar: 2}");
    REQUIRE( weather.hasJson() );
    REQUIRE( weather.json() == std::string("{foo: 1, bar: 2}") );
  }
}
