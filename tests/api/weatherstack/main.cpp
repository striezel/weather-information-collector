/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
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

#include <fstream>
#include <iostream>
#include "../../../src/api/Weatherstack.hpp"

void printWeather(const wic::Weather& w)
{
  std::cout << "Temperature: " << w.temperatureCelsius() << " °C (" << w.hasTemperatureCelsius() << ")\n"
            << "Temperature: " << w.temperatureFahrenheit() << " °F (" << w.hasTemperatureFahrenheit() << ")\n"
            << "Temperature: " << w.temperatureKelvin() << " K (" << w.hasTemperatureKelvin() << ")\n"
            << "Pressure: " << w.pressure() << " hPa (" << w.hasPressure() << ")\n"
            << "Humidity: " << static_cast<int>(w.humidity()) << " % (" << w.hasHumidity() << ")\n"
            << "Rain: " << w.rain() << " mm (" << w.hasRain() << ")\n"
            << "Snow: " << w.snow() << " mm (" << w.hasSnow() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
}

int main(int argc, char** argv)
{
  if ((argc < 3) || (argv[1] == nullptr) || (argv[2] == nullptr))
  {
    std::cerr << "Error: No JSON file name was specified!" << std::endl;
    std::cerr << "Two files are needed:\n"
              << " * JSON file for current weather data (weatherstack.current.json)" << std::endl
              << " * JSON file for snowy current weather data (weatherstack.current.snow.json)\n" << std::endl;
    return 1;
  }

  {
    const std::string jsonFileName = std::string(argv[1]);
    std::ifstream jsonStream;
    jsonStream.open(jsonFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonFileName << " could not be opened!" << std::endl;
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Weatherstack api;
    wic::Weather w;
    bool success = api.parseCurrentWeather(json, w);
    if (!success)
    {
      std::cerr << "Error: JSON data (current weather) could not be parsed!" << std::endl;
      return 1;
    }

    std::cout << "Current weather data:" << std::endl;
    printWeather(w);

    if (!w.hasHumidity())
    {
      std::cerr << "Error: Weather object does not have humidity data!\n";
      return 1;
    }
    if (!w.hasRain())
    {
      std::cerr << "Error: Weather object does not have rain amount information!\n";
      return 1;
    }
    if (!w.hasSnow())
    {
      std::cerr << "Error: Weather object does not have snow amount information!\n";
      return 1;
    }
    if (!w.hasPressure())
    {
      std::cerr << "Error: Weather object does not have pressure data!\n";
      return 1;
    }
    if (!w.hasTemperatureCelsius())
    {
      std::cerr << "Error: Weather object does not have temperature data in °C!\n";
      return 1;
    }
    if (!w.hasTemperatureFahrenheit())
    {
      std::cerr << "Error: Weather object does not have temperature data in °F!\n";
      return 1;
    }
    if (!w.hasTemperatureKelvin())
    {
      std::cerr << "Error: Weather object does not have temperature data in Kelvin!\n";
      return 1;
    }
    if (!w.hasWindDegrees())
    {
      std::cerr << "Error: Weather object does not have wind direction!\n";
      return 1;
    }
    if (!w.hasWindSpeed())
    {
      std::cerr << "Error: Weather object does not have wind speed!\n";
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

    if (w.humidity() != 82)
    {
      std::cerr << "Humidity value is incorrect.\n";
      return 1;
    }
    if (w.rain() != 0.0f)
    {
      std::cerr << "Rain amount is incorrect.\n";
      return 1;
    }
    if (w.snow() != 0.0f)
    {
      std::cerr << "Snow amount is incorrect.\n";
      return 1;
    }
    if (w.pressure() != 1009)
    {
      std::cerr << "Pressure value is incorrect.\n";
      return 1;
    }
    if (w.temperatureCelsius() != 14.0f)
    {
      std::cerr << "Temperature (°C) is incorrect.\n";
      return 1;
    }
    if (w.temperatureFahrenheit() != 57.2f)
    {
      std::cerr << "Temperature (°F) is incorrect.\n";
      return 1;
    }
    if (w.temperatureKelvin() != 287.15f)
    {
      std::cerr << "Temperature (Kelvin) is incorrect.\n";
      return 1;
    }
    if (w.windDegrees() != 150)
    {
      std::cerr << "Wind direction is incorrect.\n";
      return 1;
    }
    if (w.windSpeed() != 17.0f / 3.6f)
    {
      std::cerr << "Wind speed is incorrect.\n";
      return 1;
    }
    if (w.cloudiness() != 0)
    {
      std::cerr << "Cloudiness is incorrect.\n";
      return 1;
    }
  } // first JSON file

  // second file (snow)
  {
    const std::string jsonSnowFile = std::string(argv[2]);
    std::ifstream jsonStream;
    jsonStream.open(jsonSnowFile, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonSnowFile << " could not be opened!" << std::endl;
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Weatherstack api;
    wic::Weather w;
    bool success = api.parseCurrentWeather(json, w);
    if (!success)
    {
      std::cerr << "Error: JSON data (current weather) could not be parsed!" << std::endl;
      return 1;
    }

    std::cout << "Snow weather data:" << std::endl;
    printWeather(w);

    if (!w.hasHumidity())
    {
      std::cerr << "Error: Weather object does not have humidity data!\n";
      return 1;
    }
    if (!w.hasRain())
    {
      std::cerr << "Error: Weather object does not have rain amount information!\n";
      return 1;
    }
    if (!w.hasSnow())
    {
      std::cerr << "Error: Weather object does not have snow amount information!\n";
      return 1;
    }
    if (!w.hasPressure())
    {
      std::cerr << "Error: Weather object does not have pressure data!\n";
      return 1;
    }
    if (!w.hasTemperatureCelsius())
    {
      std::cerr << "Error: Weather object does not have temperature data in °C!\n";
      return 1;
    }
    if (!w.hasTemperatureFahrenheit())
    {
      std::cerr << "Error: Weather object does not have temperature data in °F!\n";
      return 1;
    }
    if (!w.hasTemperatureKelvin())
    {
      std::cerr << "Error: Weather object does not have temperature data in Kelvin!\n";
      return 1;
    }
    if (!w.hasWindDegrees())
    {
      std::cerr << "Error: Weather object does not have wind direction!\n";
      return 1;
    }
    if (!w.hasWindSpeed())
    {
      std::cerr << "Error: Weather object does not have wind speed!\n";
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

    if (w.humidity() != 92)
    {
      std::cerr << "Humidity value is incorrect.\n";
      return 1;
    }
    if (w.rain() != 0.0f)
    {
      std::cerr << "Rain amount is incorrect.\n";
      return 1;
    }
    if (w.snow() != 0.7f)
    {
      std::cerr << "Snow amount is incorrect.\n";
      return 1;
    }
    if (w.pressure() != 1009)
    {
      std::cerr << "Pressure value is incorrect.\n";
      return 1;
    }
    if (w.temperatureCelsius() != -4.0f)
    {
      std::cerr << "Temperature (°C) is incorrect.\n";
      return 1;
    }
    if (w.temperatureFahrenheit() != 24.8f)
    {
      std::cerr << "Temperature (°F) is incorrect.\n";
      return 1;
    }
    if (w.temperatureKelvin() != 269.15f)
    {
      std::cerr << "Temperature (Kelvin) is incorrect.\n";
      return 1;
    }
    if (w.windDegrees() != 236)
    {
      std::cerr << "Wind direction is incorrect.\n";
      return 1;
    }
    if (w.windSpeed() != 33.0f / 3.6f)
    {
      std::cerr << "Wind speed is incorrect.\n";
      return 1;
    }
    if (w.cloudiness() != 100)
    {
      std::cerr << "Cloudiness is incorrect.\n";
      return 1;
    }
  } // second JSON file (snow)

  // All tests passed.
  return 0;
}
