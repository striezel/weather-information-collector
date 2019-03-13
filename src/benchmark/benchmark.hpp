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

#ifndef WEATHER_INFORMATION_COLLECTOR_BENCHMARK_HPP
#define WEATHER_INFORMATION_COLLECTOR_BENCHMARK_HPP

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include "../api/Types.hpp"
#include "../data/Forecast.hpp"
#include "../data/Location.hpp"
#include "../data/Weather.hpp"
#include "../retrieve/SourceMySQL.hpp"
#include "../ReturnCodes.hpp"

namespace wic
{

void printWeather(const Weather& w)
{
  std::cout << "Temperature: " << w.temperatureCelsius() << " °C (" << w.hasTemperatureCelsius() << ")\n"
            << "Temperature: " << w.temperatureFahrenheit() << " °F (" << w.hasTemperatureFahrenheit() << ")\n"
            << "Temperature: " << w.temperatureKelvin() << " K (" << w.hasTemperatureKelvin() << ")\n"
            << "Humidity: " << static_cast<int>(w.humidity()) << " % (" << w.hasHumidity() << ")\n"
            << "Rain: " << w.rain() << " mm (" << w.hasRain() << ")\n"
            << "Snow: " << w.snow() << " mm (" << w.hasSnow() << ")\n"
            << "Pressure: " << w.pressure() << " hPa (" << w.hasPressure() << ")\n"
            << "Wind speed: " << w.windSpeed() << " m/s (" << w.hasWindSpeed() << ")\n"
            << "Wind direction: " << w.windDegrees() << " ° (" << w.hasWindDegrees() << ")\n"
            << "Cloudiness: " << static_cast<int>(w.cloudiness()) << " % (" << w.hasCloudiness() << ")\n";
  const std::time_t dt_c = std::chrono::system_clock::to_time_t(w.dataTime());
  std::cout << "Data time: " << std::ctime(&dt_c) << "\n";
  const std::time_t rt_c = std::chrono::system_clock::to_time_t(w.requestTime());
  std::cout << "Request time: " << std::ctime(&rt_c) << "\n";
}

void printForecast(const Forecast& f)
{
  const std::time_t rt_c = std::chrono::system_clock::to_time_t(f.requestTime());
  std::cout << "Request time: " << std::ctime(&rt_c) << "\n"
            << "Number of data points: " << f.data().size() << "\n";
  for (decltype(f.data().size()) i = 0; i < f.data().size(); ++i)
  {
    std::cout << "Data point #" << i << "\n";
    printWeather(f.data().at(i));
  } // for
}

/* ********* Weather data ********* */
template<typename jsonCppT, typename nlohmannJsonT>
int weatherDataBench(const ApiType api, SourceMySQL& source)
{
  // Find all locations with weather data.
  std::vector<std::pair<Location, ApiType>> weatherLocations;
  if (!source.listWeatherLocationsWithApi(weatherLocations))
  {
    std::cerr << "Error: Could not retrieve locations with weather data!" << std::endl;
    return rcDatabaseError;
  }

  // Erase all locations that are not retrieved with the given API.
  const auto erasableIterator =
  std::remove_if(weatherLocations.begin(), weatherLocations.end(),
              [&](const std::pair<Location, ApiType>& elem) { return elem.second != api; }
              );
  if (erasableIterator != weatherLocations.end())
  {
    weatherLocations.erase(erasableIterator, weatherLocations.end());
  }

  std::cout << "Weather data of the following locations will be parsed:\n";
  for (const auto& p : weatherLocations)
  {
    std::cout << p.first.toString() << ", " << toString(p.second) << "\n";
  }
  if (weatherLocations.size() == 0)
  {
    std::cout << "none" << std::endl << std::endl << "Exiting benchmark." << std::endl;
    return 0;
  }

  std::cout << std::endl;

  for(const auto& elem : weatherLocations)
  {
    std::vector<Weather> data;
    if (!source.getCurrentWeather(elem.second, elem.first, data))
    {
      std::cerr << "Error: Could not get weather data for "
                << elem.first.toString() << std::endl;
      return rcDatabaseError;
    }

    // Parse with JsonCpp.
    const auto jsonCppStart = std::chrono::high_resolution_clock::now();
    for (const Weather& elem: data)
    {
      if (!elem.hasJson())
        continue;

      Weather dummy;
      if (!jsonCppT::parseCurrentWeather(elem.json(), dummy))
      {
        std::cerr << "Error: Could not parse JSON with JsonCpp!" << std::endl
                  << "JSON is: '" << elem.json() << "'." << std::endl;
        return 1;
      }

      // compare with value from database
      // -- First set request time, because that one cannot be determined from
      //    the raw JSON alone.
      dummy.setRequestTime(elem.requestTime());
      if (dummy != elem)
      {
        std::cerr << "Error: JsonCpp-parsed element does not match the element from the database!" << std::endl;
        std::cerr << "json() matches: " << (elem.json() == dummy.json() ? "yes" : "no") << std::endl;
        std::cerr << "Element from database:\n";
        printWeather(elem);
        std::cerr << "Element from parser:\n";
        printWeather(dummy);
        return 42;
      }
    } // for
    const auto jsonCppEnd = std::chrono::high_resolution_clock::now();

    // Parse with nlohmann/json.
    const auto nlohmannJsonStart = std::chrono::high_resolution_clock::now();
    for (const Weather& elem: data)
    {
      if (!elem.hasJson())
        continue;

      Weather dummy;
      if (!nlohmannJsonT::parseCurrentWeather(elem.json(), dummy))
      {
        std::cerr << "Error: Could not parse JSON with nlohmann/json!" << std::endl
                  << "JSON is: '" << elem.json() << "'." << std::endl;
        return 1;
      }

      // compare with value from database
      // -- First set request time, because that one cannot be determined from
      //    the raw JSON alone.
      dummy.setRequestTime(elem.requestTime());
      if (dummy != elem)
      {
        std::cerr << "Error: nlohmann/json-parsed element does not match the element from the database!" << std::endl;
        std::cerr << "json() matches: " << (elem.json() == dummy.json() ? "yes" : "no") << std::endl;
        std::cerr << "Element from database:\n";
        printWeather(elem);
        std::cerr << "Element from parser:\n";
        printWeather(dummy);
        return 42;
      }
    } // for
    const auto nlohmannJsonEnd = std::chrono::high_resolution_clock::now();

    std::cout << "Parsing " << data.size() << " elements for "
              << elem.first.toString() << " took:" << std::endl;
    const auto jsonCppTime = std::chrono::duration_cast<std::chrono::microseconds>(jsonCppEnd - jsonCppStart);
    const auto jsonCppPerElement = static_cast<double>(jsonCppTime.count()) / data.size();
    const auto nlohmannJsonTime = std::chrono::duration_cast<std::chrono::microseconds>(nlohmannJsonEnd - nlohmannJsonStart);
    const auto nlohmannJsonPerElement = static_cast<double>(nlohmannJsonTime.count()) / data.size();
    const double percentage = static_cast<double>(nlohmannJsonTime.count()) / jsonCppTime.count() * 100.0;
    std::cout << "JsonCpp:       " << jsonCppTime.count() << " microseconds (100 %)\n"
              << "               (ca. " << jsonCppPerElement << " microseconds per element)\n"
              << "nlohmann/json: " << nlohmannJsonTime.count() << " microseconds (" << percentage << " %)\n"
              << "               (ca. " << nlohmannJsonPerElement << " microseconds per element)\n"
              << std::endl;
  } // for

  // Done, all went fine.
  return 0;
} // end of scope for weather data benchmark

/* ********* Forecast data ********* */
template<typename jsonCppT, typename nlohmannJsonT>
int forecastBench(const ApiType api, SourceMySQL& source)
{
  // Find all locations with forecast data.
  std::vector<std::pair<Location, ApiType>> forecastLocations;
  if (!source.listForecastLocationsWithApi(forecastLocations))
  {
    std::cerr << "Error: Could not retrieve locations with forecast data!" << std::endl;
    return rcDatabaseError;
  }

  // Erase all locations that are not retrieved with the given API.
  const auto erasableIterator =
  std::remove_if(forecastLocations.begin(), forecastLocations.end(),
              [&](const std::pair<Location, ApiType>& elem) { return elem.second != api; }
              );
  if (erasableIterator != forecastLocations.end())
  {
    forecastLocations.erase(erasableIterator, forecastLocations.end());
  }

  std::cout << "Forecast data of the following locations will be parsed:\n";
  for (const auto& p : forecastLocations)
  {
    std::cout << p.first.toString() << ", " << toString(p.second) << "\n";
  }
  if (forecastLocations.size() == 0)
  {
    std::cout << "none" << std::endl << std::endl << "Exiting benchmark." << std::endl;
    return 0;
  }

  std::cout << std::endl;

  for(const auto& elem : forecastLocations)
  {
    std::vector<Forecast> data;
    if (!source.getForecasts(elem.second, elem.first, data))
    {
      std::cerr << "Error: Could not get weather forecast data for "
                << elem.first.toString() << std::endl;
      return rcDatabaseError;
    }

    // Parse with JsonCpp.
    const auto jsonCppStart = std::chrono::high_resolution_clock::now();
    for (const Forecast& elem: data)
    {
      if (!elem.hasJson())
        continue;

      Forecast dummy;
      if (!jsonCppT::parseForecast(elem.json(), dummy))
      {
        std::cerr << "Error: Could not parse JSON with JsonCpp!" << std::endl
                  << "JSON is: '" << elem.json() << "'." << std::endl;
        return 1;
      }

      // compare with value from database
      // -- First set request time, because that one cannot be determined from
      //    the raw JSON alone.
      dummy.setRequestTime(elem.requestTime());
      if (dummy != elem)
      {
        std::cerr << "Error: JsonCpp-parsed element does not match the element from the database!" << std::endl;
        std::cerr << "json() matches: " << (elem.json() == dummy.json() ? "yes" : "no") << std::endl;
        std::cerr << "JSON is '" << elem.json() << "'." << std::endl;
        std::cerr << "Element from database:\n";
        printForecast(elem);
        std::cerr << "Element from parser:\n";
        printForecast(dummy);
        return 42;
      }
    } // for
    const auto jsonCppEnd = std::chrono::high_resolution_clock::now();

    // Parse with nlohmann/json.
    const auto nlohmannJsonStart = std::chrono::high_resolution_clock::now();
    for (const Forecast& elem: data)
    {
      if (!elem.hasJson())
        continue;

      Forecast dummy;
      if (!nlohmannJsonT::parseForecast(elem.json(), dummy))
      {
        std::cerr << "Error: Could not parse JSON with nlohmann/json!" << std::endl
                  << "JSON is: '" << elem.json() << "'." << std::endl;
        return 1;
      }

      // compare with value from database
      // -- First set request time, because that one cannot be determined from
      //    the raw JSON alone.
      dummy.setRequestTime(elem.requestTime());
      if (dummy != elem)
      {
        std::cerr << "Error: nlohmann/json-parsed element does not match the element from the database!" << std::endl;
        std::cerr << "json() matches: " << (elem.json() == dummy.json() ? "yes" : "no") << std::endl;
        std::cerr << "Element from database:\n";
        printForecast(elem);
        std::cerr << "Element from parser:\n";
        printForecast(dummy);
        return 42;
      }
    } // for
    const auto nlohmannJsonEnd = std::chrono::high_resolution_clock::now();

    std::cout << "Parsing " << data.size() << " elements for "
              << elem.first.toString() << " took:" << std::endl;
    const auto jsonCppTime = std::chrono::duration_cast<std::chrono::microseconds>(jsonCppEnd - jsonCppStart);
    const auto jsonCppPerElement = static_cast<double>(jsonCppTime.count()) / data.size();
    const auto nlohmannJsonTime = std::chrono::duration_cast<std::chrono::microseconds>(nlohmannJsonEnd - nlohmannJsonStart);
    const auto nlohmannJsonPerElement = static_cast<double>(nlohmannJsonTime.count()) / data.size();
    const double percentage = static_cast<double>(nlohmannJsonTime.count()) / jsonCppTime.count() * 100.0;
    std::cout << "JsonCpp:       " << jsonCppTime.count() << " microseconds (100 %)\n"
              << "               (ca. " << jsonCppPerElement << " microseconds per element)\n"
              << "nlohmann/json: " << nlohmannJsonTime.count() << " microseconds (" << percentage << " %)\n"
              << "               (ca. " << nlohmannJsonPerElement << " microseconds per element)\n"
              << std::endl;
  } // for

  // Done, no errors.
  return 0;
} // end of scope for forecast data benchmark

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_BENCHMARK_HPP
