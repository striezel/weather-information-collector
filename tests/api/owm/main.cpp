/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2018  Dirk Stolle

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
#include "../../../src/data/Forecast.hpp"

void printWeather(const wic::Weather& w)
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
}

int main(int argc, char** argv)
{
  if ((argc < 4) || (argv[1] == nullptr) || (argv[2] == nullptr) || (argv[3] == nullptr))
  {
    std::cerr << "Error: No JSON file name was specified!\n";
    std::cerr << "Three files are needed:\n"
              << " * JSON file for current weather data (owm.current.json)\n"
              << " * JSON file for forecast weather data (owm.forecast.json)\n"
              << " * JSON file for snowy forecast data (owm.forecast.snow.json)" << std::endl;
    return 1;
  }

  wic::OpenWeatherMap api;

  // test for current weather data
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

    if (w.hasRain())
    {
      std::cerr << "Error: Weather object has rain data, but it should not!\n";
      return 1;
    }

    if (w.hasSnow())
    {
      std::cerr << "Error: Weather object has snow data, but it should not!\n";
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

    if (!w.hasTemperatureFahrenheit())
    {
      std::cerr << "Error: Weather object does not have temperature data in °F, but it should!\n";
      return 1;
    }
    if ((w.temperatureFahrenheit() > 66.201f) || (w.temperatureFahrenheit() < 66.199f))
    {
      std::cerr << "Temperature (°F) is incorrect. Should be 19.0 °F, but it is " << w.temperatureFahrenheit() << "°F!\n";
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
      std::cerr << "Error: JSON data with rain could not be parsed!\n";
      return 1;
    }

    printWeather(w);
    const std::time_t dt_c2 = std::chrono::system_clock::to_time_t(w.dataTime());
    std::cout << "Data time: " << std::ctime(&dt_c2) << "\n";

    if (!w.hasRain())
    {
      std::cerr << "Error: Weather object does not have rain amount information!\n";
      return 1;
    }
    if (w.rain() != 17.08f)
    {
      std::cerr << "Rain amount is incorrect.\n";
      return 1;
    }
    if (w.hasSnow())
    {
      std::cerr << "Error: Weather object has snow data, but it should not!\n";
      return 1;
    }
  }

  // Read forecast file.
  {
    const std::string jsonForecastFile = std::string(argv[2]);
    std::ifstream jsonStream;
    jsonStream.open(jsonForecastFile, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file with forecast data could not be opened!\n";
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    if (!api.parseForecast(json, forecast))
    {
      std::cerr << "Error: JSON forecast data could not be parsed!\n";
      return 1;
    }

    if (forecast.data().size() != 40)
    {
      std::cerr << "Error: JSON forecast data contains " << forecast.data().size()
                << "elements, but 40 elements were expected!\n";
      return 1;
    }

    const wic::Weather w = forecast.data().at(0);
    std::cout << "1st data element of forecast:\n";
    printWeather(w);
    const std::time_t dt_fc = std::chrono::system_clock::to_time_t(w.dataTime());
    std::cout << "Data time: " << std::ctime(&dt_fc) << "\n";
    std::cout << "Data time (UTC): " << std::asctime(std::gmtime(&dt_fc)) << "\n";

    if (w.temperatureKelvin() != 300.38f)
    {
      std::cerr << "Temperature (Kelvin) is incorrect.\n";
      return 1;
    }
    if ((w.temperatureCelsius() > 27.231f) || (w.temperatureCelsius() < 27.229f))
    {
      std::cerr << "Temperature (°C) is incorrect. Should be 27.23 °C, but it is " << w.temperatureCelsius() << " °C!\n";
      return 1;
    }
    if (w.pressure() != 993)
    {
      std::cerr << "Pressure value is incorrect.\n";
      return 1;
    }
    if (w.humidity() != 58)
    {
      std::cerr << "Humidity value is incorrect.\n";
      return 1;
    }
    if (w.windSpeed() != 2.06f)
    {
      std::cerr << "Wind speed is incorrect.\n";
      return 1;
    }
    if (w.windDegrees() != 92)
    {
      std::cerr << "Wind direction is incorrect.\n";
      return 1;
    }
  }

  // Read forecast file with snow data.
  {
    const std::string jsonForecastSnowFile = std::string(argv[3]);
    std::ifstream jsonStream;
    jsonStream.open(jsonForecastSnowFile, std::ios_base::in | std::ios_base::binary);
    if (!jsonStream.is_open())
    {
      std::cerr << "Error: JSON file with snowy forecast data could not be opened!\n";
      return 1;
    }

    std::string json;
    std::getline(jsonStream, json, '\0');
    jsonStream.close();

    wic::Forecast forecast;
    if (!api.parseForecast(json, forecast))
    {
      std::cerr << "Error: JSON forecast data could not be parsed!\n";
      return 1;
    }

    if (forecast.data().size() != 40)
    {
      std::cerr << "Error: JSON forecast data contains " << forecast.data().size()
                << "elements, but 40 elements were expected!\n";
      return 1;
    }

    // zeroth data element: rain, but no snow
    {
      const wic::Weather& zeroth = forecast.data().at(0);
      std::cout << "1st data element of snowy forecast:\n";
      printWeather(zeroth);
      // Should only have rain data, no snow.
      if (!zeroth.hasRain())
      {
        std::cerr << "Error: Weather object zero does not have rain amount information!\n";
        return 1;
      }
      if (zeroth.rain() != 0.095f)
      {
        std::cerr << "Rain amount of zeroth object is incorrect.\n";
        return 1;
      }
      if (zeroth.hasSnow())
      {
        std::cerr << "Error: Weather object zero has snow data, but it should not!\n";
        return 1;
      }
    }

    //2nd element: empty rain object, no snow data
    {
      const wic::Weather& second = forecast.data().at(2);
      std::cout << "3rd data element of snowy forecast:\n";
      printWeather(second);
      // Should only have rain data.
      if (!second.hasRain())
      {
        std::cerr << "Error: Weather object two does not have rain amount information!\n";
        return 1;
      }
      if (second.rain() != 0.0f)
      {
        std::cerr << "Rain amount of second object is incorrect.\n";
        return 1;
      }
      if (second.hasSnow())
      {
        std::cerr << "Error: Weather object two has snow data, but it should not!\n";
        return 1;
      }
    }


    // 10th data element: rain and snow
    {
      const wic::Weather& tenth = forecast.data().at(10);
      std::cout << "11th data element of snowy forecast:\n";
      printWeather(tenth);
      // Should only have rain data.
      if (!tenth.hasRain())
      {
        std::cerr << "Error: Weather object ten does not have rain amount information!\n";
        return 1;
      }
      if (tenth.rain() != 0.025f)
      {
        std::cerr << "Rain amount of tenth object is incorrect.\n";
        return 1;
      }
      if (!tenth.hasSnow())
      {
        std::cerr << "Error: Weather object ten has no snow data, but it should have!\n";
        return 1;
      }
      if (tenth.snow() != 0.09f)
      {
        std::cerr << "Snow amount of tenth object is incorrect.\n";
        return 1;
      }
    }

    // 39th data element: empty rain and empty snow object
    {
      const wic::Weather& elem39 = forecast.data().at(39);
      std::cout << "39th data element of snowy forecast:\n";
      printWeather(elem39);
      // Should have rain data.
      if (!elem39.hasRain())
      {
        std::cerr << "Error: Weather object 39 does not have rain amount information!\n";
        return 1;
      }
      if (elem39.rain() != 0.0f)
      {
        std::cerr << "Rain amount of 39th object is incorrect.\n";
        return 1;
      }
      if (!elem39.hasSnow())
      {
        std::cerr << "Error: Weather object 39 has no snow data, but it should have!\n";
        return 1;
      }
      if (elem39.snow() != 0.0f)
      {
        std::cerr << "Snow amount of 39th object is incorrect.\n";
        return 1;
      }
    }
  }

  // All tests passed.
  return 0;
}
