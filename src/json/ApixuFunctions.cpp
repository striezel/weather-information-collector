/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2018  Dirk Stolle

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

#include "ApixuFunctions.hpp"

namespace wic
{

void precipitationDistinction(const float amount, Weather& weather)
{
  // Apixu provides no direct distinction between rain and snow. There are
  // the condition texts, but checking those is tedious and error prone,
  // because they may change. Therefore we have to do something else.
  // Basic idea: If temperature is above zero, it is rain. Below zero it
  // is snow, because water freezes at zero °C.
  if (weather.temperatureCelsius() >= 0.0f)
  {
    weather.setRain(amount);
    weather.setSnow(0.0f);
  }
  else if (weather.temperatureCelsius() < 0.0f)
  {
    weather.setRain(0.0f);
    weather.setSnow(amount);
  }
  else
  {
    // Temperature (°C) is NaN, so let's have a guess and go with rain.
    weather.setRain(amount);
    weather.setSnow(0.0f);
  }
}

} // namespace
