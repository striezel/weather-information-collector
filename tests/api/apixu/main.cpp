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
#include "../../../src/api/Apixu.hpp"


void printWeather(const wic::Weather& w)
{
  std::cout << "Temperature: " << w.temperatureCelsius() << " °C (" << w.hasTemperatureCelsius() << ")\n"
            << "Temperature: " << w.temperatureFahrenheit() << " °F (" << w.hasTemperatureFahrenheit() << ")\n"
            << "Pressure: " << w.pressure() << " hPa (" << w.hasPressure() << ")\n"
            << "Humidity: " << static_cast<int>(w.humidity()) << " % (" << w.hasHumidity() << ")\n"
            << "Rain: " << w.rain() << " mm (" << w.hasRain() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
}

int main(int argc, char** argv)
{
  if ((argc < 3) || (argv[1] == nullptr))
  {
    std::cerr << "Error: No JSON file names were specified!\n";
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

  wic::Apixu api;
  wic::Weather w;
  const bool success = api.parseCurrentWeather(json, w);
  if (!success)
  {
    std::cerr << "Error: JSON data could not be parsed!\n";
    return 1;
  }

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

  if (w.humidity() != 83)
  {
    std::cerr << "Humidity value is incorrect.\n";
    return 1;
  }
  if (w.rain() != 0.8f)
  {
    std::cerr << "Rain amount is incorrect.\n";
    return 1;
  }
  if (w.pressure() != 1019)
  {
    std::cerr << "Pressure value is incorrect.\n";
    return 1;
  }
  if (w.temperatureCelsius() != 19.0f)
  {
    std::cerr << "Temperature (°C) is incorrect.\n";
    return 1;
  }
  if (w.temperatureFahrenheit() != 66.2f)
  {
    std::cerr << "Temperature (°F) is incorrect.\n";
    return 1;
  }
  if (w.windDegrees() != 240)
  {
    std::cerr << "Wind direction is incorrect.\n";
    return 1;
  }
  if (w.windSpeed() != (6.8f / 3.6f))
  {
    std::cerr << "Wind speed is incorrect.\n";
    return 1;
  }
  if (w.cloudiness() != 0)
  {
    std::cerr << "Cloudiness is incorrect.\n";
    return 1;
  }

  //test for rain data

  std::string jsonRainFileName = jsonFileName;
  jsonRainFileName.insert(jsonRainFileName.size() - 5, ".rain");
  jsonStream.open(jsonRainFileName, std::ios_base::in | std::ios_base::binary);
  if (!jsonStream.is_open())
  {
    std::cerr << "Error: JSON file " << jsonRainFileName << " could not be opened!\n";
    return 1;
  }

  std::getline(jsonStream, json, '\0');
  jsonStream.close();

  const bool successRain = api.parseCurrentWeather(json, w);
  if (!successRain)
  {
    std::cerr << "Error: JSON data could not be parsed!\n";
    return 1;
  }

  printWeather(w);

  if (!w.hasRain())
  {
    std::cerr << "Error: Weather object does not have rain amount information!\n";
    return 1;
  }
  if (w.rain() != 1.1f)
  {
    std::cerr << "Rain amount is incorrect.\n";
    return 1;
  }

  // Read forecast data file.
  const std::string jsonForecastFileName = std::string(argv[2]);
  jsonStream.open(jsonForecastFileName, std::ios_base::in | std::ios_base::binary);
  if (!jsonStream.is_open())
  {
    std::cerr << "Error: JSON file " << jsonForecastFileName << " could not be opened!\n";
    return 1;
  }
  json.clear();
  std::getline(jsonStream, json, '\0');
  jsonStream.close();

  wic::Forecast forecast;
  if (!api.parseForecast(json, forecast))
  {
    std::cerr << "Error: JSON forecast data from " << jsonForecastFileName
              << " could not be parsed!" << std::endl;
    return 1;
  }
  if (forecast.data().size() != 14)
  {
    std::cerr << "Error: Forecast data should contain 14 entries, but there are "
              << forecast.data().size() << " entries instead!" << std::endl;
    return 1;
  }
  {
    const auto& w1 = forecast.data().at(0);
    std::cout << "Data of 1st forecast item:" << std::endl;
    printWeather(w1);
    if (w1.temperatureCelsius() != 17.0f)
    {
      std::cerr << "Temperature (°C) of 1st forecast item is incorrect.\n";
      return 1;
    }
    if (w1.temperatureFahrenheit() != 62.6f)
    {
      std::cerr << "Temperature (°F) of 1st forecast item is incorrect.\n";
      return 1;
    }
    if (w1.rain() != 0.1f)
    {
      std::cerr << "Rain amount of 1st forecast item is incorrect.\n";
      return 1;
    }
    if (w1.humidity() != 53)
    {
      std::cerr << "Relative humidity of 1st forecast item is incorrect.\n";
      return 1;
    }

    const auto& w2 = forecast.data().at(1);
    std::cout << "Data of 2nd forecast item:" << std::endl;
    printWeather(w2);
    if (w2.temperatureCelsius() != 26.0f)
    {
      std::cerr << "Temperature (°C) of 2nd forecast item is incorrect.\n";
      return 1;
    }
    if (w2.temperatureFahrenheit() != 78.8f)
    {
      std::cerr << "Temperature (°F) of 2nd forecast item is incorrect.\n";
      return 1;
    }
    if (w2.rain() != 0.1f)
    {
      std::cerr << "Rain amount of 2nd forecast item is incorrect.\n";
      return 1;
    }
    if (w2.humidity() != 53)
    {
      std::cerr << "Relative humidity of 2nd forecast item is incorrect.\n";
      return 1;
    }
  } // scope

  // All tests passed.
  return 0;
}
