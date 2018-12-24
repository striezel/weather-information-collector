/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
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
    /** \brief Default constructor, creates instance without any data. */
    Weather();


    /** \brief Checks whether this instance has time when the data was received / measured.
     *
     * \return Returns true, if the instance has the time.
     *         Returns false otherwise.
     */
    bool hasDataTime() const;


    /** \brief Gets the time when the data was received / measured.
     *
     * \return Returns the time when the data was received / measured.
     *         Returns the epoch, if no time is set.
     */
    const std::chrono::time_point<std::chrono::system_clock>& dataTime() const;


    /** \brief Sets the time when the data was received / measured.
     *
     * \param dt  the new time when the data was received / measured
     */
    void setDataTime(const std::chrono::time_point<std::chrono::system_clock>& dt);


    /** \brief Checks whether this instance has time when the API request was performed.
     *
     * \return Returns true, if the instance has the time when then API request was performed.
     *         Returns false otherwise.
     */
    bool hasRequestTime() const;


    /** \brief Sets the time when the API request was performed.
     *
     * \param rt  the new time when the API request was performed
     */
    void setRequestTime(const std::chrono::time_point<std::chrono::system_clock>& rt);


    /** \brief Gets the time when the API request was performed.
     *
     * \return Returns the time when the API request was performed.
     *         Returns the epoch, if no time is set.
     */
    const std::chrono::time_point<std::chrono::system_clock>& requestTime() const;


    /** \brief Checks whether this instance has a temperature in Kelvin.
     *
     * \return Returns true, if the instance has a temperature in Kelvin.
     *         Returns false otherwise.
     */
    bool hasTemperatureKelvin() const;


    /** \brief Gets the temperature in Kelvin, if it was set.
     *
     * \return Returns the temperature in Kelvin.
     *         Returns NaN, if no temperature is set for that unit.
     */
    float temperatureKelvin() const;


    /** \brief Sets the temperature in Kelvin.
     *
     * \param newTempK  the new temperature in Kelvin
     */
    void setTemperatureKelvin(const float newTempK);


    /** \brief Checks whether this instance has a temperature in °C.
     *
     * \return Returns true, if the instance has a temperature in °C.
     *         Returns false otherwise.
     */
    bool hasTemperatureCelsius() const;


    /** \brief Gets the temperature in °C, if it was set.
     *
     * \return Returns the temperature in °C.
     *         Returns NaN, if no temperature is set for that unit.
     */
    float temperatureCelsius() const;


    /** \brief Sets the temperature in °C.
     *
     * \param newTempC  the new temperature in °C
     */
    void setTemperatureCelsius(const float newTempC);


    /** \brief Checks whether this instance has a temperature in °F.
     *
     * \return Returns true, if the instance has a temperature in °F.
     *         Returns false otherwise.
     */
    bool hasTemperatureFahrenheit() const;


    /** \brief Gets the temperature in °F, if it was set.
     *
     * \return Returns the temperature in °F.
     *         Returns NaN, if no temperature is set for that unit.
     */
    float temperatureFahrenheit() const;


    /** \brief Sets the temperature in °F.
     *
     * \param newTempF  the new temperature in °F
     */
    void setTemperatureFahrenheit(const float newTempF);


    /** \brief Checks whether this instance has a relative humidity.
     *
     * \return Returns true, if the instance has humidity information.
     *         Returns false otherwise.
     */
    bool hasHumidity() const;


    /** \brief Gets the relative humidity in percent, if it was set.
     *
     * \return Returns the relative humidity in percent.
     *         Returns -1, if no humidity is set.
     */
    int8_t humidity() const;


    /** \brief Sets the humidity in percent.
     *
     * \param newHumidity  the new humidity in percent
     */
    void setHumidity(const int8_t newHumidity);


    /** \brief Checks whether this instance has information about rain.
     *
     * \return Returns true, if the instance has rain information.
     *         Returns false otherwise.
     */
    bool hasRain() const;


    /** \brief Gets the amount of rain in millimeters, if it was set.
     *
     * \return Returns the amount of rain in millimeters.
     *         Returns NaN, if no rain information is set.
     */
    float rain() const;


    /** \brief Sets the amount of rain in millimeters.
     *
     * \param newRainMm  the new amount of rain in millimeters
     */
    void setRain(const float newRainMm);


    /** \brief Checks whether this instance has information about snow.
     *
     * \return Returns true, if the instance has snow information.
     *         Returns false otherwise.
     */
    bool hasSnow() const;


    /** \brief Gets the amount of snow in millimeters, if it was set.
     *
     * \return Returns the amount of snow in millimeters.
     *         Returns NaN, if no snow information is set.
     */
    float snow() const;


    /** \brief Sets the amount of snow in millimeters.
     *
     * \param newSnowMm  the new amount of snow in millimeters
     */
    void setSnow(const float newSnowMm);


    /** \brief Checks whether this instance has air pressure information.
     *
     * \return Returns true, if the instance has air pressure.
     *         Returns false otherwise.
     */
    bool hasPressure() const;


    /** \brief Gets the air pressure in hPa, if it was set.
     *
     * \return Returns the pressure in hPa.
     *         Returns -1, if no pressure is set.
     */
    int16_t pressure() const;


    /** \brief Sets the pressure in hPa.
     *
     * \param newPressure_hPa  the new pressure in hPa
     */
    void setPressure(const int16_t newPressure_hPa);


    /** \brief Checks whether this instance has a wind speed.
     *
     * \return Returns true, if the instance has a wind speed.
     *         Returns false otherwise.
     */
    bool hasWindSpeed() const;


    /** \brief Gets the wind speed in meters per second, if it was set.
     *
     * \return Returns the wind speed in meters per second.
     *         Returns NaN, if no wind speed is set.
     */
    float windSpeed() const;


    /** \brief Sets the wind speed in m/s.
     *
     * \param newSpeed  the new wind speed in meters per second
     */
    void setWindSpeed(const float newSpeed);


    /** \brief Checks whether this instance has a wind direction.
     *
     * \return Returns true, if the instance has a wind direction.
     *         Returns false otherwise.
     */
    bool hasWindDegrees() const;


    /** \brief Gets the wind direction in degrees, if it was set.
     *
     * \return Returns the wind direction in degrees.
     *         Returns -1, if no wind direction is set.
     */
    int16_t windDegrees() const;


    /** \brief Sets the wind direction degrees (0°=N, 90°=E,...).
     *
     * \param newWindDegrees  the new wind direction in degrees
     */
    void setWindDegrees(const int16_t newWindDegrees);


    /** \brief Checks whether this instance has cloudiness information.
     *
     * \return Returns true, if the instance has cloudiness information.
     *         Returns false otherwise.
     */
    bool hasCloudiness() const;


    /** \brief Gets the cloudiness in percent, if it was set.
     *
     * \return Returns the cloudiness in percent.
     *         Returns -1, if no cloudiness is set.
     */
    int8_t cloudiness() const;


    /** \brief Sets the cloudiness in percent.
     *
     * \param newCloudiness  the new cloudiness in percent
     */
    void setCloudiness(const int8_t newCloudiness);


    /** \brief Checks whether this instance has raw JSON data.
     *
     * \return Returns true, if the instance has raw JSON data.
     *         Returns false otherwise.
     */
    bool hasJson() const;


    /** \brief Gets the raw JSON data.
     *
     * \return Returns the raw JSON data as string.
     *         Returns an empty string, if no JSON data is set.
     */
    const std::string& json() const;


    /** \brief Sets the raw JSON data.
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
    int8_t m_humidity; /**< relative humidity in percent */
    float m_rain; /**< amount of rain within the last three hours in mm */
    float m_snow; /**< amount of snow within the last three hours in mm */
    int16_t m_pressure; /**< air pressure in hPa */
    float m_windSpeed; /**< wind speed in meters per second */
    int16_t m_windDegrees; /**< wind direction in degrees */
    int8_t m_cloudiness; /**< cloudiness in percent */
    std::string m_json; /**< raw JSON data */
}; //class

} //namespace

#endif // WEATHER_INFORMATION_COLLECTOR_WEATHER_HPP
