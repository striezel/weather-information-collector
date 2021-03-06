/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2019, 2020  Dirk Stolle

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
#include "../../../src/api/Weatherbit.hpp"

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
  if ((argc < 5) || (argv[1] == nullptr) || (argv[2] == nullptr) || (argv[3] == nullptr) || (argv[4] == nullptr))
  {
    std::cerr << "Error: No JSON file name was specified!" << std::endl;
    std::cerr << "Four files are needed:\n"
              << " * JSON file for current weather data (weatherbit.current.json)\n"
              << " * JSON file for hourly weather forecast data (weatherbit.forecast.hourly.json)\n"
              << " * JSON file for 3-hourly weather forecast data (weatherbit.forecast.3hourly.json)\n"
              << " * JSON file for daily weather forecast data (weatherbit.forecast.daily.json)" << std::endl;
    return 1;
  }
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

  wic::Weatherbit api(wic::PlanWeatherbit::Free);
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

  if (w.humidity() != 42)
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
  if (w.pressure() != 1005)
  {
    std::cerr << "Pressure value is incorrect.\n";
    return 1;
  }
  if (w.temperatureCelsius() != 16.4f)
  {
    std::cerr << "Temperature (°C) is incorrect.\n";
    return 1;
  }
  if (w.temperatureFahrenheit() != 61.52f)
  {
    std::cerr << "Temperature (°F) is incorrect.\n";
    return 1;
  }
  if (w.temperatureKelvin() != 289.55f)
  {
    std::cerr << "Temperature (Kelvin) is incorrect.\n";
    return 1;
  }
  if (w.windDegrees() != 264)
  {
    std::cerr << "Wind direction is incorrect.\n";
    return 1;
  }
  if (w.windSpeed() != 2.00069f)
  {
    std::cerr << "Wind speed is incorrect.\n";
    return 1;
  }
  if (w.cloudiness() != 72)
  {
    std::cerr << "Cloudiness is incorrect.\n";
    return 1;
  }

  // Test hourly forecast data.
  {
    const std::string jsonForecastFileName = std::string(argv[2]);

    jsonStream.open(jsonForecastFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonForecastFileName << " could not be opened!" << std::endl;
      return 1;
    }
    json.clear();
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    success = api.parseForecast(json, forecast);
    if (!success)
    {
      std::cerr << "Error: JSON data (current weather) could not be parsed!" << std::endl;
      return 1;
    }
    if (forecast.data().size() != 48) // one entry per hour for the next two days
    {
      std::cerr << "Error: Hourly forecast data should contain 48 entries, but there are "
                << forecast.data().size() << " entries instead!" << std::endl;
      return 1;
    }

    std::cout << "First hourly forecast data:" << std::endl;
    const auto& wf0 = forecast.data().at(0);
    printWeather(wf0);
    if (!wf0.hasDataTime())
    {
      std::cerr << "Hourly forecast item has no timestamp.\n";
      return 1;
    }
    if (wf0.rain() != 0.0f)
    {
      std::cerr << "Rain amount of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.snow() != 0.0f)
    {
      std::cerr << "Snow amount of hourly forecast item is not set to zero.\n";
      return 1;
    }
    if (wf0.temperatureCelsius() != 15.9f)
    {
      std::cerr << "Temperature (°C) of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.temperatureFahrenheit() != 60.62f)
    {
      std::cerr << "Temperature (°F) of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.humidity() != 51)
    {
      std::cerr << "Humidity of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.pressure() != 1004)
    {
      std::cerr << "Air pressure of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.windSpeed() < 1.83082f || wf0.windSpeed() > 1.83084f)
    {
      std::cerr << "Wind speed of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.windDegrees() != 253)
    {
      std::cerr << "Wind direction of hourly forecast item is incorrect.\n";
      return 1;
    }
    if (wf0.cloudiness() != 13)
    {
      std::cerr << "Cloudiness of hourly forecast item is incorrect.\n";
      return 1;
    }
  }

  // Test forecast weather data with 3 hours interval.
  {
    const std::string json3hourlyFileName = std::string(argv[3]);
    jsonStream.open(json3hourlyFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << json3hourlyFileName << " could not be opened!" << std::endl;
      return 1;
    }
    json.clear();
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    success = api.parseForecast(json, forecast);
    if (!success)
    {
      std::cerr << "Error: JSON data (forecast weather, 3 hour intervals) could not be parsed!" << std::endl;
      return 1;
    }
    if (forecast.data().size() != 40) // one entry every three hours for the next five days
    {
      std::cerr << "Error: Hourly forecast data should contain 40 entries, but there are "
                << forecast.data().size() << " entries instead!" << std::endl;
      return 1;
    }

    // Check forecast weather part.
    // -- first entry
    std::cout << "Forecast weather, 3 hours interval, first element:" << std::endl;
    const auto& zeroth = forecast.data().at(0);
    printWeather(zeroth);
    if (!zeroth.hasDataTime())
    {
      std::cerr << "Three-hourly forecast item has no timestamp.\n";
      return 1;
    }
    if (zeroth.rain() != 0.0f)
    {
      std::cerr << "Rain amount of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.snow() != 0.0f)
    {
      std::cerr << "Snow amount of 3-hourly forecast item is not set to zero.\n";
      return 1;
    }
    if (zeroth.temperatureCelsius() != 12.8f)
    {
      std::cerr << "Temperature (°C) of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.temperatureFahrenheit() != 55.04f)
    {
      std::cerr << "Temperature (°F) of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.humidity() != 60)
    {
      std::cerr << "Humidity of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.pressure() != 1004)
    {
      std::cerr << "Air pressure of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.windSpeed() < 0.959622f || zeroth.windSpeed() > 0.959624f)
    {
      std::cerr << "Wind speed of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.windDegrees() != 240)
    {
      std::cerr << "Wind direction of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
    if (zeroth.cloudiness() != 1)
    {
      std::cerr << "Cloudiness of 3-hourly forecast item is incorrect.\n";
      return 1;
    }
  }

  // Test forecast weather data with daily interval.
  {
    const std::string jsonDailyFileName = std::string(argv[4]);
    jsonStream.open(jsonDailyFileName, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file " << jsonDailyFileName << " could not be opened!" << std::endl;
      return 1;
    }
    json.clear();
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    success = api.parseForecast(json, forecast);
    if (!success)
    {
      std::cerr << "Error: JSON data (forecast weather, daily intervals) could not be parsed!" << std::endl;
      return 1;
    }
    if (forecast.data().size() != 16) // one entry every day for the next 16 days
    {
      std::cerr << "Error: Daily forecast data should contain 16 entries, but there are "
                << forecast.data().size() << " entries instead!" << std::endl;
      return 1;
    }

    // Check forecast weather part.
    // -- first entry
    std::cout << "Forecast weather, daily interval, first element:" << std::endl;
    {
      const auto &zeroth = forecast.data().at(0);
      printWeather(zeroth);
      if (!zeroth.hasDataTime())
      {
        std::cerr << "Daily forecast item has no timestamp.\n";
        return 1;
      }
      if (zeroth.rain() != 0.0161133f)
      {
        std::cerr << "Rain amount of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.snow() != 0.0f)
      {
        std::cerr << "Snow amount of daily forecast item is not set to zero.\n";
        return 1;
      }
      if (zeroth.temperatureCelsius() != 4.1f)
      {
        std::cerr << "Temperature (°C) of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.temperatureFahrenheit() != 39.38f)
      {
        std::cerr << "Temperature (°F) of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.humidity() != 76)
      {
        std::cerr << "Humidity of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.pressure() != 1002)
      {
        std::cerr << "Air pressure of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.windSpeed() < 2.74763f || zeroth.windSpeed() > 2.74765f)
      {
        std::cerr << "Wind speed of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.windDegrees() != 281)
      {
        std::cerr << "Wind direction of daily forecast item is incorrect.\n";
        return 1;
      }
      if (zeroth.cloudiness() != 52)
      {
        std::cerr << "Cloudiness of daily forecast item is incorrect.\n";
        return 1;
      }
    }

    // -- 15th entry
    std::cout << "Forecast weather, daily interval, 16th element:" << std::endl;
    {
      const auto &sixteenth = forecast.data().at(15);
      printWeather(sixteenth);
      if (!sixteenth.hasDataTime())
      {
        std::cerr << "Daily forecast item 16 has no timestamp.\n";
        return 1;
      }
      if (sixteenth.rain() != 0.0f)
      {
        std::cerr << "Rain amount of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.snow() != 0.0f)
      {
        std::cerr << "Snow amount of daily forecast item 16 is not set to zero.\n";
        return 1;
      }
      if (sixteenth.temperatureCelsius() != 3.0f)
      {
        std::cerr << "Temperature (°C) of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.temperatureFahrenheit() != 37.4f)
      {
        std::cerr << "Temperature (°F) of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.humidity() != 61)
      {
        std::cerr << "Humidity of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.pressure() != 996)
      {
        std::cerr << "Air pressure of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.windSpeed() < 3.70262f || sixteenth.windSpeed() > 3.70264f)
      {
        std::cerr << "Wind speed of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.windDegrees() != 72)
      {
        std::cerr << "Wind direction of daily forecast item 16 is incorrect.\n";
        return 1;
      }
      if (sixteenth.cloudiness() != 14)
      {
        std::cerr << "Cloudiness of daily forecast item 16 is incorrect.\n";
        return 1;
      }
    }
  }

  // All tests passed.
  return 0;
}
