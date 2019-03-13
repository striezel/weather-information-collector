/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector benchmark tool.
    Copyright (C) 2019  Dirk Stolle

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

#ifndef WEATHER_INFORMATION_COLLECTOR_TEST_OWM_LOCATION_HPP
#define WEATHER_INFORMATION_COLLECTOR_TEST_OWM_LOCATION_HPP

#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "../../../src/data/LocationWithCountry.hpp"
#include "../../../src/data/Weather.hpp"

namespace wic
{

void printWeather(const Weather& w)
{
  std::cout << "Temperature: " << w.temperatureKelvin() << " K (" << w.hasTemperatureKelvin() << ")\n"
            << "Temperature: " << w.temperatureCelsius() << " °C (" << w.hasTemperatureCelsius() << ")\n"
            << "Temperature: " << w.temperatureFahrenheit() << " °F (" << w.hasTemperatureFahrenheit() << ")\n"
            << "Pressure: " << w.pressure() << " hPa (" << w.hasPressure() << ")\n"
            << "Humidity: " << static_cast<int>(w.humidity()) << " % (" << w.hasHumidity() << ")\n"
            << "Rain: " << w.rain() << " mm (" << w.hasRain() << ")\n"
            << "Snow: " << w.snow() << " mm (" << w.hasSnow() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
  const std::time_t rt_c = std::chrono::system_clock::to_time_t(w.requestTime());
  std::cout << "Request time: " << std::ctime(&rt_c) << "\n";
}

template<typename jsonT>
int parseLocations(const std::string& json)
{
  std::vector<std::pair<LocationWithCountry, Weather> > locations;

  if (!jsonT::parseLocations(json, locations))
  {
    std::cerr << "Error: JSON data could not be parsed!\n";
    return 1;
  }

  // Check location data.
  if (locations.size() != 5)
  {
    std::cerr << "Error: Expected five locations, but there are "
              << locations.size() << " locations instead!" << std::endl;
    return 1;
  }

  // first entry
  {
    const LocationWithCountry& location = locations.at(0).first;
    const Weather& weather = locations.at(0).second;

    if (location.country() != "DE")
    {
      std::cerr << "Error: Expected country DE, but it is "
                << location.country() << " instead!" << std::endl;
      return 1;
    }
    if (location.id() != 2950159)
    {
      std::cerr << "Error: Expected id 2950159, but it is "
                << location.id() << " instead!" << std::endl;
      return 1;
    }
    if (location.name() != "Berlin")
    {
      std::cerr << "Error: Expected name 'Berlin', but it is '"
                << location.name() << "' instead!" << std::endl;
      return 1;
    }
    if (location.hasPostcode())
    {
      std::cerr << "Error: Expected no postcode, but there is one!" << std::endl;
      return 1;
    }
    if (std::fabs(location.latitude() - 52.517f) > 0.001)
    {
      std::cerr << "Error: Expected latitude of 52.517 °, but it is "
                << location.latitude() << " ° instead!" << std::endl;
      return 1;
    }
    if (std::fabs(location.longitude() - 13.3889f) > 0.001)
    {
      std::cerr << "Error: Expected longitude of 13.3889 °, but it is "
                << location.longitude() << " ° instead!" << std::endl;
      return 1;
    }

    Weather expectedWeather;
    expectedWeather.setTemperatureKelvin(284.07);
    expectedWeather.setTemperatureCelsius(284.07 - 273.15);
    expectedWeather.setTemperatureFahrenheit((284.07 - 273.15) * 1.8 + 32.0);
    expectedWeather.setHumidity(71);
    expectedWeather.setPressure(1005);
    expectedWeather.setWindSpeed(7.7);
    expectedWeather.setWindDegrees(230);
    expectedWeather.setRain(0.0f);
    expectedWeather.setCloudiness(75);
    expectedWeather.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1551612084)));
    if (expectedWeather != weather)
    {
      std::cerr << "Error: Weather data of first item does not match expected weather!" << std::endl;
      std::cout << "Expected weather:" << std::endl;
      printWeather(expectedWeather);
      std::cout << "Parsed weather:" << std::endl;
      printWeather(weather);
      return 1;
    }
  } // 1st entry

  // second entry
  {
    const LocationWithCountry& location = locations.at(1).first;
    const Weather& weather = locations.at(1).second;

    if (location.country() != "US")
    {
      std::cerr << "Error in 2nd entry: Expected country US, but it is "
                << location.country() << " instead!" << std::endl;
      return 1;
    }

    Location expectedLocation;
    expectedLocation.setId(4831725);
    expectedLocation.setCoordinates(41.6215, -72.7457);
    expectedLocation.setName("Berlin");
    expectedLocation.setPostcode("");

    if (expectedLocation != location)
    {
      std::cerr << "Error: Weather data of 2nd item does not match expected weather!" << std::endl;
      return 1;
    }

    Weather expectedWeather;
    expectedWeather.setTemperatureKelvin(271.02);
    expectedWeather.setTemperatureCelsius(-2.13);
    expectedWeather.setTemperatureFahrenheit((271.02 - 273.15) * 1.8 + 32.0);
    expectedWeather.setHumidity(100);
    expectedWeather.setPressure(1020);
    expectedWeather.setWindSpeed(2.66);
    expectedWeather.setWindDegrees(321.502);
    //expectedWeather.setRain(NaN);
    expectedWeather.setCloudiness(20);
    expectedWeather.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1551612156)));
    if (expectedWeather != weather)
    {
      std::cerr << "Error: Weather data of second item does not match expected weather!" << std::endl;
      std::cout << "Expected weather:" << std::endl;
      printWeather(expectedWeather);
      std::cout << "Parsed weather:" << std::endl;
      printWeather(weather);
      return 1;
    }
  }

  // third entry
  {
    const LocationWithCountry& location = locations.at(2).first;
    const Weather& weather = locations.at(2).second;

    if (location.country() != "US")
    {
      std::cerr << "Error in 3rd entry: Expected country US, but it is "
                << location.country() << " instead!" << std::endl;
      return 1;
    }

    Location expectedLocation;
    expectedLocation.setId(5163076);
    expectedLocation.setCoordinates(40.5612, -81.7944);
    expectedLocation.setName("Berlin");
    expectedLocation.setPostcode("");

    if (expectedLocation != location)
    {
      std::cerr << "Error: Weather data of 3rd item does not match expected weather!" << std::endl;
      return 1;
    }

    Weather expectedWeather;
    expectedWeather.setTemperatureKelvin(270.99);
    expectedWeather.setTemperatureCelsius(270.99 - 273.15);
    expectedWeather.setTemperatureFahrenheit((270.99 - 273.15) * 1.8 + 32.0);
    expectedWeather.setHumidity(80);
    expectedWeather.setPressure(1021);
    expectedWeather.setWindSpeed(1.5);
    expectedWeather.setWindDegrees(50);
    //expectedWeather.setRain(NaN);
    expectedWeather.setCloudiness(90);
    expectedWeather.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1551612072)));
    if (expectedWeather != weather)
    {
      std::cerr << "Error: Weather data of 3rd item does not match expected weather!" << std::endl;
      std::cout << "Expected weather:" << std::endl;
      printWeather(expectedWeather);
      std::cout << "Parsed weather:" << std::endl;
      printWeather(weather);
      return 1;
    }
  }

  // fourth entry
  {
    const LocationWithCountry& location = locations.at(3).first;
    const Weather& weather = locations.at(3).second;

    if (location.country() != "ZA")
    {
      std::cerr << "Error in 4th entry: Expected country ZA, but it is "
                << location.country() << " instead!" << std::endl;
      return 1;
    }

    Location expectedLocation;
    expectedLocation.setId(1019330);
    expectedLocation.setCoordinates(-32.8834, 27.5794);
    expectedLocation.setName("Berlin");
    expectedLocation.setPostcode("");

    if (expectedLocation != location)
    {
      std::cerr << "Error: Weather data of 4th item does not match expected weather!" << std::endl;
      return 1;
    }

    Weather expectedWeather;
    expectedWeather.setTemperatureKelvin(298.15);
    expectedWeather.setTemperatureCelsius(298.15 - 273.15);
    expectedWeather.setTemperatureFahrenheit((298.15 - 273.15) * 1.8 + 32.0);
    expectedWeather.setHumidity(65);
    expectedWeather.setPressure(1015);
    expectedWeather.setWindSpeed(9.8);
    expectedWeather.setWindDegrees(70);
    //expectedWeather.setRain(NaN);
    expectedWeather.setCloudiness(20);
    expectedWeather.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1551610800)));
    if (expectedWeather != weather)
    {
      std::cerr << "Error: Weather data of 4th item does not match expected weather!" << std::endl;
      std::cout << "Expected weather:" << std::endl;
      printWeather(expectedWeather);
      std::cout << "Parsed weather:" << std::endl;
      printWeather(weather);
      return 1;
    }
  }

  // fifth entry
  {
    const LocationWithCountry& location = locations.at(4).first;
    const Weather& weather = locations.at(4).second;

    if (location.country() != "DE")
    {
      std::cerr << "Error in 5th entry: Expected country DE, but it is "
                << location.country() << " instead!" << std::endl;
      return 1;
    }

    Location expectedLocation;
    expectedLocation.setId(2950158);
    expectedLocation.setCoordinates(54.0364, 10.4461);
    expectedLocation.setName("Berlin");
    expectedLocation.setPostcode("");

    if (expectedLocation != location)
    {
      std::cerr << "Error: Weather data of 5th item does not match expected weather!" << std::endl;
      return 1;
    }

    Weather expectedWeather;
    expectedWeather.setTemperatureKelvin(282.57);
    expectedWeather.setTemperatureCelsius(282.57 - 273.15);
    expectedWeather.setTemperatureFahrenheit((282.57 - 273.15) * 1.8 + 32.0);
    expectedWeather.setHumidity(93);
    expectedWeather.setPressure(1002);
    expectedWeather.setWindSpeed(5.7);
    expectedWeather.setWindDegrees(250);
    expectedWeather.setRain(0.0f);
    expectedWeather.setCloudiness(75);
    expectedWeather.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1551612101)));
    if (expectedWeather != weather)
    {
      std::cerr << "Error: Weather data of 5th item does not match expected weather!" << std::endl;
      std::cout << "Expected weather:" << std::endl;
      printWeather(expectedWeather);
      std::cout << "Parsed weather:" << std::endl;
      printWeather(weather);
      return 1;
    }
  }

  std::cout << "Location parsing was successful and data matches the expectations." << std::endl;
  return 0;
}

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_TEST_OWM_LOCATION_HPP
