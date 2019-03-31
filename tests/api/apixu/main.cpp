/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2018, 2019  Dirk Stolle

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

#include <cmath>
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
            << "Snow: " << w.snow() << " mm (" << w.hasSnow() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
}

int main(int argc, char** argv)
{
  if ((argc < 6) || (argv[1] == nullptr) || (argv[2] == nullptr)
    || (argv[3] == nullptr) || (argv[4] == nullptr) || (argv[5] == nullptr))
  {
    std::cerr << "Error: No JSON file names were specified!\n";
    std::cerr << "Five files are needed:\n"
              << " * JSON file for current weather data (apixu.current.json)\n"
              << " * JSON file for forecast weather data (apixu.forecast.json)\n"
              << " * JSON file for hourly forecast weather data (apixu.forecast.hour.json)\n"
              << " * JSON file for snowy current weather data (apixu.current.snow.json)\n"
              << " * JSON file for snowy forecast data (apixu.forecast.snow.json)" << std::endl;
    return 1;
  }

  wic::Apixu api;

  const std::string jsonFileName = std::string(argv[1]);
  {
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
    if (!w.hasSnow())
    {
      std::cerr << "Error: Weather object does not have snow amount information, but it should have!\n";
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
    if (w.snow() != 0.0f)
    {
      std::cerr << "Snow amount is incorrect.\n";
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
    if (std::fabs(w.windSpeed() - (6.8f / 3.6f)) > 0.000001f)
    {
      std::cerr << "Wind speed is incorrect.\n";
      return 1;
    }
    if (w.cloudiness() != 0)
    {
      std::cerr << "Cloudiness is incorrect.\n";
      return 1;
    }
  }

  // test for rain data
  {
    std::string jsonRainFileName = jsonFileName;
    jsonRainFileName.insert(jsonRainFileName.size() - 5, ".rain");
    std::ifstream jsonStream;
    jsonStream.open(jsonRainFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonRainFileName << " could not be opened!\n";
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Weather w;
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
    if (!w.hasSnow())
    {
      std::cerr << "Error: Weather object does not have snow amount information, but it should have!\n";
      return 1;
    }
    if (w.snow() != 0.0f)
    {
      std::cerr << "Snow amount is not zero!\n";
      return 1;
    }
  }

  // Read forecast data file.
  {
    const std::string jsonForecastFileName = std::string(argv[2]);
    std::ifstream jsonStream;
    jsonStream.open(jsonForecastFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonForecastFileName << " could not be opened!\n";
      return 1;
    }
    std::string json;
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
    if (w1.snow() != 0.0f)
    {
      std::cerr << "Snow amount of 1st forecast item is incorrect.\n";
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
    if (!w2.hasSnow())
    {
      std::cerr << "Error: 2nd forecast item should have snow amount information, but it has not!\n";
      return 1;
    }
    if (w2.snow() != 0.0f)
    {
      std::cerr << "Snow amount of 2nd forecast item is incorrect.\n";
      return 1;
    }
    if (w2.humidity() != 53)
    {
      std::cerr << "Relative humidity of 2nd forecast item is incorrect.\n";
      return 1;
    }
  } // scope


  // Read forecast data file with hourly forecast.
  {
    const std::string jsonForecastHourlyFileName = std::string(argv[3]);
    std::ifstream jsonStream;
    jsonStream.open(jsonForecastHourlyFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonForecastHourlyFileName << " could not be opened!\n";
      return 1;
    }
    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    if (!api.parseForecast(json, forecast))
    {
      std::cerr << "Error: JSON forecast data from " << jsonForecastHourlyFileName
                << " could not be parsed!" << std::endl;
      return 1;
    }
    if (forecast.data().size() != 24)
    {
      std::cerr << "Error: Hourly forecast data should contain 24 entries, but there are "
                << forecast.data().size() << " entries instead!" << std::endl;
      return 1;
    }

    std::cout << "First hourly forecast data:" << std::endl;
    const auto& w3 = forecast.data().at(0);
    printWeather(w3);
    if (!w3.hasDataTime())
    {
      std::cerr << "Hourly forecast item has no timestamp.\n";
      return 1;
    }
    if (w3.temperatureCelsius() != 1.2f)
    {
      std::cerr << "Temperature (°C) of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.temperatureFahrenheit() != 34.2f)
    {
      std::cerr << "Temperature (°F) of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.windSpeed() < 3.694f || w3.windSpeed() > 3.6945f)
    {
      std::cerr << "Wind speed of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.windDegrees() != 86)
    {
      std::cerr << "Wind direction of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.pressure() != 993.0f)
    {
      std::cerr << "Air pressure of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.rain() != 0.6f)
    {
      std::cerr << "Rain amount of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (!w3.hasSnow())
    {
      std::cerr << "Error: Hourly forecast item does not have snow amount information, but it should have!\n";
      return 1;
    }
    if (w3.snow() != 0.0f)
    {
      std::cerr << "Snow amount of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.humidity() != 98)
    {
      std::cerr << "Humidity of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (w3.cloudiness() != 100)
    {
      std::cerr << "Cloudiness of hourly forecast item is incorrect.\n";
      return 1;
    }
  }

  // Read current weather data file with snow.
  {
    const std::string jsonSnowFileName = std::string(argv[4]);
    std::ifstream jsonStream;
    jsonStream.open(jsonSnowFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonSnowFileName << " could not be opened!\n";
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Weather w;
    const bool success = api.parseCurrentWeather(json, w);
    if (!success)
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
    if (!w.hasSnow())
    {
      std::cerr << "Error: Weather object does not have snow amount information, but it should have!\n";
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

    if (w.rain() != 0.0f)
    {
      std::cerr << "Rain amount is incorrect.\n";
      return 1;
    }
    if (w.snow() != 0.1f)
    {
      std::cerr << "Snow amount is incorrect.\n";
      return 1;
    }
    if (w.temperatureCelsius() != -28.0f)
    {
      std::cerr << "Temperature (°C) is incorrect.\n";
      return 1;
    }
    if (w.temperatureFahrenheit() != -18.4f)
    {
      std::cerr << "Temperature (°F) is incorrect.\n";
      return 1;
    }
  }

  // Read forecast data file with snow.
  {
    const std::string jsonSnowFileName = std::string(argv[5]);
    std::ifstream jsonStream;
    jsonStream.open(jsonSnowFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonSnowFileName << " could not be opened!\n";
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    if (!api.parseForecast(json, forecast))
    {
      std::cerr << "Error: JSON forecast data from " << jsonSnowFileName
                << " could not be parsed!" << std::endl;
      return 1;
    }
    if (forecast.data().size() != 2)
    {
      std::cerr << "Error: Forecast data should contain two entries, but there are "
                << forecast.data().size() << " entries instead!" << std::endl;
      return 1;
    }

    const auto& w1 = forecast.data().at(0);
    std::cout << "Data of 1st snowy forecast item:" << std::endl;
    printWeather(w1);
    if (w1.temperatureCelsius() != -33.6f)
    {
      std::cerr << "Temperature (°C) of 1st snowy forecast item is incorrect.\n";
      return 1;
    }
    if (w1.temperatureFahrenheit() != -28.5f)
    {
      std::cerr << "Temperature (°F) of 1st snowy forecast item is incorrect.\n";
      return 1;
    }
    if (w1.rain() != 0.0f)
    {
      std::cerr << "Rain amount of 1st snowy forecast item is incorrect.\n";
      return 1;
    }
    if (w1.snow() != 0.8f)
    {
      std::cerr << "Snow amount of 1st snowy forecast item is incorrect.\n";
      return 1;
    }

    const auto& w2 = forecast.data().at(1);
    std::cout << "Data of 2nd snowy forecast item:" << std::endl;
    printWeather(w2);
    if (w2.temperatureCelsius() != -25.6f)
    {
      std::cerr << "Temperature (°C) of 2nd snowy forecast item is incorrect.\n";
      return 1;
    }
    if (w2.temperatureFahrenheit() != -14.1f)
    {
      std::cerr << "Temperature (°F) of 2nd snowy forecast item is incorrect.\n";
      return 1;
    }
    if (w2.rain() != 0.0f)
    {
      std::cerr << "Rain amount of 2nd snowy forecast item is incorrect.\n";
      return 1;
    }
    if (!w2.hasSnow())
    {
      std::cerr << "Error: 2nd snowy forecast item should have snow amount information, but it has not!\n";
      return 1;
    }
    if (w2.snow() != 0.8f)
    {
      std::cerr << "Snow amount of 2nd snowy forecast item is incorrect.\n";
      return 1;
    }
    if (w2.humidity() != 91)
    {
      std::cerr << "Relative humidity of 2nd snowy forecast item is incorrect.\n";
      return 1;
    }
  }

  // All tests passed.
  return 0;
}
