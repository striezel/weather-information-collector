/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2019, 2021  Dirk Stolle

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

#include "WeatherstackFunctions.hpp"

namespace wic
{

void weatherstackPrecipitationDistinction(const float amount, Weather& weather, const int code)
{
  // Weatherstack provides no direct distinction between rain and snow. There
  // are the condition texts, but checking those is tedious and error prone,
  // because they may change. Therefore we have to do something else.

  // First idea: Use known weather codes.
  switch (code)
  {
    case 176: // Patchy rain nearby
    case 200: // Thundery outbreaks in nearby
    case 248: // Fog
    case 263: // Patchy light drizzle
    case 266: // Light drizzle
    case 293: // Patchy light rain
    case 296: // Light rain
    case 299: // Moderate rain at times
    case 302: // Moderate rain
    case 305: // Heavy rain at times
    case 308: // Heavy rain
    case 353: // Light rain shower
    case 356: // Moderate or heavy rain shower
    case 359: // Torrential rain shower
    case 386: // Patchy light rain in area with thunder
    case 389: // Moderate or heavy rain in area with thunder
         weather.setRain(amount);
         weather.setSnow(0.0f);
         return;
    case 179: // Patchy snow nearby
    case 227: // Blowing snow
    case 230: // Blizzard
    case 260: // Freezing fog
    case 323: // Patchy light snow
    case 326: // Light snow
    case 329: // Patchy moderate snow
    case 332: // Moderate snow
    case 335: // Patchy heavy snow
    case 338: // Heavy snow
    case 350: // Ice pellets
    case 368: // Light snow showers
    case 371: // Moderate or heavy snow showers
    case 374: // Light showers of ice pellets
    case 377: // Moderate or heavy showers of ice pellets
    case 392: // Patchy light snow in area with thunder
    case 395: // Moderate or heavy snow in area with thunder
         weather.setRain(0.0f);
         weather.setSnow(amount);
         return;
    case 182: // Patchy sleet nearby
    case 317: // Light sleet
    case 320: // Moderate or heavy sleet
    case 362: // Light sleet showers
    case 365: // Moderate or heavy sleet showers
         // Sleet could be described as mixture of snow and rain.
         weather.setRain(0.5f * amount);
         weather.setSnow(0.5f * amount);
         return;
    default:
         // No known code.
         // Second idea: If temperature is above zero, it is rain. Below zero it
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
           if (amount == 0.0f)
           {
             weather.setSnow(0.0f);
           }
         }
         return;
  } // switch
}

} // namespace
