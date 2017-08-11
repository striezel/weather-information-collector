/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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

#ifndef WEATHER_INFORMATION_COLLECTOR_WEATHER_HPP
#define WEATHER_INFORMATION_COLLECTOR_WEATHER_HPP

#include <chrono>
#include <cstdint>
#include <string>

namespace wic
{

class Weather
{
  public:
    /** \brief default constructor */
    Weather();


    /** \brief checks whether this instance has time when the data was received / measured
     *
     * \return Returns true, if the instance has the time.
     *         Returns false otherwise.
     */
    bool hasDataTime() const;


    /** \brief gets the time when the data was received / measured
     *
     * \return Returns the time when the data was received / measured.
     *         Returns the epoch, if no time is set.
     */
    const std::chrono::time_point<std::chrono::system_clock>& dataTime() const;


    /** \brief sets the time when the data was received / measured
     *
     * \param dt  the new time when the data was received / measured
     */
    void setDataTime(const std::chrono::time_point<std::chrono::system_clock>& dt);


    /** \brief checks whether this instance has time when the API request was performed
     *
     * \return Returns true, if the instance has the time when then API request was performed.
     *         Returns false otherwise.
     */
    bool hasRequestTime() const;


    /** \brief sets the time when the API request was performed
     *
     * \param rt  the new time when the API request was performed
     */
    void setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt);


    /** \brief gets the time when the API request was performed
     *
     * \return Returns the time when the API request was performed.
     *         Returns the epoch, if no time is set.
     */
    const std::chrono::time_point<std::chrono::system_clock>& requestTime() const;


    /** \brief checks whether this instance has a temperature in Kelvin
     *
     * \return Returns true, if the instance has a temperature in Kelvin.
     *         Returns false otherwise.
     */
    bool hasTemperatureKelvin() const;


    /** \brief gets the temperature in Kelvin, if it was set
     *
     * \return Returns the temperature in Kelvin.
     *         Returns NaN, if no temperature is set for that unit.
     */
    float temperatureKelvin() const;


    /** \brief sets the temperature in Kelvin
     *
     * \param newTempK  the new temperature in Kelvin
     */
    void setTemperatureKelvin(const float newTempK);


    /** \brief checks whether this instance has a temperature in Kelvin
     *
     * \return Returns true, if the instance has a temperature in Kelvin.
     *         Returns false otherwise.
     */
    bool hasTemperatureCelsius() const;


    /** \brief gets the temperature in °C, if it was set
     *
     * \return Returns the temperature in °C.
     *         Returns NaN, if no temperature is set for that unit.
     */
    float temperatureCelsius() const;


    /** \brief sets the temperature in °C
     *
     * \param newTempC  the new temperature in °C
     */
    void setTemperatureCelsius(const float newTempC);


    /** \brief checks whether this instance has a temperature in °F
     *
     * \return Returns true, if the instance has a temperature in °F.
     *         Returns false otherwise.
     */
    bool hasTemperatureFahrenheit() const;


    /** \brief gets the temperature in °F, if it was set
     *
     * \return Returns the temperature in °F.
     *         Returns NaN, if no temperature is set for that unit.
     */
    float temperatureFahrenheit() const;


    /** \brief sets the temperature in °F
     *
     * \param newTempF  the new temperature in °F
     */
    void setTemperatureFahrenheit(const float newTempF);


    /** \brief checks whether this instance has a wind speed
     *
     * \return Returns true, if the instance has a wind speed.
     *         Returns false otherwise.
     */
    bool hasWindSpeed() const;


    /** \brief gets the wind speed in meters per second, if it was set
     *
     * \return Returns the wind speed in meters per second.
     *         Returns NaN, if no wind speed is set.
     */
    float windSpeed() const;


    /** \brief sets the temperature in m/s
     *
     * \param newSpeed  the new wind speed in meters per second
     */
    void setWindSpeed(const float newSpeed);


    /** \brief checks whether this instance has a wind direction
     *
     * \return Returns true, if the instance has a wind direction.
     *         Returns false otherwise.
     */
    bool hasWindDegrees() const;


    /** \brief gets the wind direction in degrees, if it was set
     *
     * \return Returns the wind direction in degrees.
     *         Returns NaN, if no wind direction is set.
     */
    float windDegrees() const;


    /** \brief sets the wind direction degrees (0°=N, 90°=E,...)
     *
     * \param newWindDegrees  the new wind direction in degrees
     */
    void setWindDegrees(const float newWindDegrees);


    /** \brief checks whether this instance has cloudiness information
     *
     * \return Returns true, if the instance has cloudiness information.
     *         Returns false otherwise.
     */
    bool hasCloudiness() const;


    /** \brief gets the cloudiness in percent, if it was set
     *
     * \return Returns the cloudiness in percent.
     *         Returns a value larger than 100, if no cloudiness is set.
     */
    uint8_t cloudiness() const;


    /** \brief sets the cloudiness in percent
     *
     * \param newCloudiness  the new cloudiness in percent
     */
    void setCloudiness(const float newCloudiness);


    /** \brief checks whether this instance has raw JSON data
     *
     * \return Returns true, if the instance has raw JSON data.
     *         Returns false otherwise.
     */
    bool hasJson() const;


    /** \brief gets the raw JSON data
     *
     * \return Returns the raw JSON data as string.
     *         Returns an empty string, if no JSON data is set.
     */
    const std::string& json() const;


    /** \brief sets the raw JSON data
     *
     * \param newJson  the new JSON data
     */
    void setJson(const std::string& newJson);
  private:
    std::chrono::time_point<std::chrono::system_clock> m_dataTime; /**< time when the data was received / measured */
    std::chrono::time_point<std::chrono::system_clock> m_requestTime; /**< time when the API request was performed */
    float m_tempK; /**< temperature in Kelvin */
    float m_tempC; /**< temperature in degrees Celsius */
    float m_tempF; /**< temperature in degrees Fahrenheit */
    float m_windSpeed; /**< wind speed in meters per second */
    uint16_t m_windDegrees; /**< wind direction in degrees */
    uint8_t m_cloudiness; /**< cloudiness in percent */
    std::string m_json; /**< raw JSON data */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_WEATHER_HPP
