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

#include <fstream>
#include <iostream>
#include "../../../src/api/OpenWeatherMap.hpp"

int main(int argc, char** argv)
{
  if ((argc < 2) || (argv[1] == nullptr))
  {
    std::cerr << "Error: No JSON file name was specified!\n";
    return 1;
  }
  const std::string jsonFileName = std::string(argv[1]);

  std::ifstream jsonStream;
  jsonStream.open(jsonFileName, std::ios_base::in | std::ios_base::binary);
  if (!jsonStream.is_open())
  {
    std::cerr << "Error: JSON file could not be opened!\n";
    return 1;
  }

  std::string json;
  std::getline(jsonStream, json, '\0');
  jsonStream.close();

  wic::OpenWeatherMap api;
  wic::Weather w;
  const bool success = api.parseCurrentWeather(json, w);
  if (!success)
  {
    std::cerr << "Error: JSON data could not be parsed!\n";
    return 1;
  }

  std::cout << "Temperature: " << w.temperatureKelvin() << " K (" << w.hasTemperatureKelvin() << ")\n"
            << "Temperature: " << w.temperatureCelsius() << " °C (" << w.hasTemperatureCelsius() << ")\n"
            << "Temperature: " << w.temperatureFahrenheit() << " °F (" << w.hasTemperatureFahrenheit() << ")\n"
            << "Pressure: " << w.pressure() << " hPa (" << w.hasPressure() << ")\n"
            << "Humidity: " << static_cast<int>(w.humidity()) << " % (" << w.hasHumidity() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";


  if (!w.hasHumidity())
  {
    std::cerr << "Error: Weather object does not have humidity data!\n";
    return 1;
  }
  if (w.humidity() != 88)
  {
    std::cerr << "Humidity value is incorrect.\n";
    return 1;
  }

  if (!w.hasPressure())
  {
    std::cerr << "Error: Weather object does not have pressure data!\n";
    return 1;
  }
  if (w.pressure() != 1018)
  {
    std::cerr << "Pressure value is incorrect.\n";
    return 1;
  }

  if (!w.hasTemperatureKelvin())
  {
    std::cerr << "Error: Weather object does not have temperature data in Kelvin!\n";
    return 1;
  }
  if (w.temperatureKelvin() != 292.15f)
  {
    std::cerr << "Temperature (Kelvin) is incorrect.\n";
    return 1;
  }

  if (!w.hasTemperatureCelsius())
  {
    std::cerr << "Error: Weather object does not have temperature data in °C, but it should!\n";
    return 1;
  }
  if ((w.temperatureCelsius() > 19.001f) || (w.temperatureCelsius() < 18.999f))
  {
    std::cerr << "Temperature (°C) is incorrect. Should be 19.0 °C, but it is " << w.temperatureCelsius() << "°C!\n";
    return 1;
  }

  if (w.hasTemperatureFahrenheit())
  {
    std::cerr << "Error: Weather object does have temperature data in °F, but it should not!\n";
    return 1;
  }

  if (!w.hasWindDegrees())
  {
    std::cerr << "Error: Weather object does not have wind direction!\n";
    return 1;
  }
  if (w.windDegrees() != 220)
  {
    std::cerr << "Wind direction is incorrect.\n";
    return 1;
  }

  if (!w.hasWindSpeed())
  {
    std::cerr << "Error: Weather object does not have wind speed!\n";
    return 1;
  }
  if (w.windSpeed() != 2.6f)
  {
    std::cerr << "Wind speed is incorrect.\n";
    return 1;
  }

  if (!w.hasCloudiness())
  {
    std::cerr << "Error: Weather object does not have cloudiness information!\n";
    return 1;
  }
  if (w.cloudiness() != 0)
  {
    std::cerr << "Cloudiness is incorrect.\n";
    return 1;
  }

  if (!w.hasDataTime())
  {
    std::cerr << "Error: Weather object does not have data time!\n";
    return 1;
  }
  if (!w.hasJson())
  {
    std::cerr << "Error: Weather object does not have raw JSON data!\n";
    return 1;
  }

  //all tests passed
  return 0;
}