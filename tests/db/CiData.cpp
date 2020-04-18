/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2020  Dirk Stolle

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

#include "CiData.hpp"
#include <chrono>

namespace wic
{

Location CiData::getDresden()
{
  wic::Location loc;
  loc.setOwmId(2935022);
  loc.setCoordinates(51.0493, 13.7381);
  loc.setName("Dresden");
  loc.setCountryCode("DE");
  return loc;
}

Location CiData::getStaplehurst()
{
  wic::Location loc;
  loc.setOwmId(2637016);
  loc.setCoordinates(51.161098, 0.552500);
  loc.setName("Staplehurst");
  loc.setCountryCode("GB");
  return loc;
}

Location CiData::getTiksi()
{
  wic::Location loc;
  loc.setOwmId(2015306);
  loc.setCoordinates(71.6366, 128.868);
  loc.setName("Tiksi");
  loc.setCountryCode("RU");
  return loc;
}

Weather CiData::sampleWeatherDresden()
{
  Weather w;
  w.setDataTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1530307200)));
  w.setRequestTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1530309564)));
  w.setTemperatureKelvin(292.15);
  w.setTemperatureCelsius(19.0);
  w.setTemperatureFahrenheit(66.2);
  w.setHumidity(68);
  w.setPressure(1014);
  w.setWindSpeed(4.6);
  w.setWindDegrees(20);
  w.setCloudiness(0);
  w.setJson(R"json(
            {"coord":{"lon":13.74,"lat":51.05},"weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01n"}],"base":"stations","main":{"temp":292.15,"pressure":1014,"humidity":68,"temp_min":292.15,"temp_max":292.15},"visibility":10000,"wind":{"speed":4.6,"deg":20},"clouds":{"all":0},"dt":1530307200,"sys":{"type":1,"id":4879,"message":0.0129,"country":"DE","sunrise":1530240828,"sunset":1530300196},"id":2935022,"name":"Dresden","cod":200}
            )json"
            );
  return w;
}

Forecast CiData::sampleForecastDresden()
{
  Forecast forecast;
  forecast.setRequestTime(std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(1543091893)));
  forecast.setJson(R"json({"this": "is not", "some": "real JSON from an API call"})json");
  forecast.setData(generate(sampleWeatherDresden()));
  return forecast;
}

std::vector<Weather> CiData::generate(Weather start, const std::size_t n, const std::chrono::seconds diff)
{
  std::vector<Weather> result;
  result.reserve(n);
  // Clear JSON element and request time, because in forecast the elements do
  // not have this kind of data.
  start.setJson("");
  start.setRequestTime(std::chrono::time_point<std::chrono::system_clock>());

  for (std::size_t i = 0; i < n; ++i)
  {
    Weather next = start;
    next.setDataTime(start.dataTime() + i * diff);
    next.setPressure(start.pressure() + i);
    next.setTemperatureCelsius(start.temperatureCelsius() + 0.25 * i);
    result.push_back(next);
  }

  return result;
}

} // namespace
