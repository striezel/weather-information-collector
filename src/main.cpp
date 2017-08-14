/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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

#include <iostream>
#include <ctime>
#include "api/OpenWeatherMap.hpp"

int main(int argc, char** argv)
{
  std::cout << "Not implemented yet!" << std::endl;

  if (argc > 2)
  {
    const std::string apiKey = std::string(argv[1]);

    wic::Location location;
    location.setName(std::string(argv[2]));

    wic::Weather weather;
    wic::OpenWeatherMap owm(apiKey);

    if (!owm.currentWeather(location, weather))
    {
      std::cout << "Failed to get current weather for " << location.name() << ".\n";
      return 1;
    }

    std::cout << "Current weather for " << location.name() << ":\n";
    const auto kelvin = weather.temperatureKelvin();
    std::cout << "Temperature: " << kelvin << " K (" << weather.hasTemperatureKelvin() << ")\n"
              << "      in °C: " << (kelvin - 273.15) << " °C\n"
              << "Pressure: " << weather.pressure() << " hPa (" << weather.hasPressure() << ")\n"
              << "Humidity: " << static_cast<int>(weather.humidity()) << " % (" << weather.hasHumidity() << ")\n"
              << "Wind speed: " << weather.windSpeed() << " m/s (" << weather.hasWindSpeed() << ")\n"
              << "Wind direction: " << weather.windDegrees() << " ° (" << weather.hasWindDegrees() << ")\n";
    const std::time_t dt_c = std::chrono::system_clock::to_time_t(weather.dataTime());
    std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
  } //if

  return 1;
}
