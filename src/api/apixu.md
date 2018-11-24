# apixu.com API

## Authentication

Authentication is achieved by passing an API key as request parameter "key",
e.g. https://api.apixu.com/v1/current.json?key=API-key-here&q=auto:ip.

## Formats

The API provides two formats, JSON and XML. Just change the extenstion to .json
or .xml, as required:

* https://api.apixu.com/v1/current.**json**?key=API-key-here&q=auto:ip
* https://api.apixu.com/v1/current.**xml**?key=API-key-here&q=auto:ip

## Error codes

Errors are indicated by returning an error object in JSON and setting a HTTP
status code. Error objects look like this:

    {
        "error":
        {
            "code": 2006,
            "message": "API key is invalid."
        }
    }

Possible HTTP status and error codes are:


| HTTP Status Code | Error code | Description                                 |
|------------------|------------|---------------------------------------------|
| 401              | 1002       | API key not provided.                       |
| 400              | 1003       | Parameter 'q' not provided.                 |
| 400              | 1005       | API request url is invalid                  |
| 400              | 1006       | No location found matching parameter 'q'    |
| 401              | 2006       | API key provided is invalid                 |
| 403              | 2007       | API key has exceeded calls per month quota. |
| 403              | 2008       | API key has been disabled.                  |
| 400              | 9999       | Internal application error.                 |

## Current weather information

Current weather information can be requested via the
<https://api.apixu.com/v1/current.json?key=API-key-here&q=location-here>
endpoint.

### Location specifier

The location can be specified in several ways:

* city name, e.g. `q=London`
* latitude and longitude in degrees, e.g. `q=51.5,12.3`
* post code
  * Canada postal code, e.g. `q=G2J`
  * UK postcode, e.g. `q=SW1`
  * US zip code, e.g. `q=10001`
* IP address
  * automatic IP geolocation lookup: `q=auto:ip`
  * IP address, e.g. `q=123.45.67.89`
* two other codes: metar code and 3 digit airport code

### Example output (JSON format)

    {
        "location":
        {
            "name": "Halle An Der Saale",
            "region": "Sachsen-Anhalt",
            "country": "Germany",
            "lat": 51.5,
            "lon": 12,
            "tz_id": "Europe/Berlin",
            "localtime_epoch": 1502160264,
            "localtime": "2017-08-08 4:44"
        },
        "current":
        {
            "last_updated_epoch": 1502159407,
            "last_updated": "2017-08-08 04:30",
            "temp_c": 14,
            "temp_f": 57.2,
            "is_day": 0,
            "condition":
            {
                "text": "Clear",
                "icon": "//cdn.apixu.com/weather/64x64/night/113.png",
                "code": 1000
            },
            "wind_mph": 9.4,
            "wind_kph": 15.1,
            "wind_degree": 100,
            "wind_dir": "E",
            "pressure_mb": 1015,
            "pressure_in": 30.4,
            "precip_mm": 0,
            "precip_in": 0,
            "humidity": 77,
            "cloud": 0,
            "feelslike_c": 12.8,
            "feelslike_f": 55.1,
            "vis_km": 10,
            "vis_miles": 6
        }
    }

### Relevant weather information

* current.last_updated: (string) time when the data was updated (YYYY-MM-DD HH:ii)
* current.temp_c: (float) temperature in °C
* current.feelslike_c: (float) what the temperature feels like in °C
* current.condition.text: (string) text describing the current condition
* current.wind_kph: (float) wind speed in km/h
* current.wind_dir: (string) wind direction as 16 point compass
* current.wind_degree: (int) wind direction in degrees (0=N, 90=E, 180=S, 270=W)
* current.pressure_mb: (float) pressure in millibars
* current.precip_mm: (float) preipitation amount in millimeters per m²
* current.humidity: (int) humidity in percent
* current.cloud: (int) cloud coverage as percentage
* current.vis_km: (float) visibility in km

## Weather forecast information

Data for weather forecasts can be requested via the
<https://api.apixu.com/v1/forecast.json?days=N&key=you-API-key-here&q=location-here>
endpoint.

In that URL, `N` is the number of days for which you want forecasts. The number
of days is limited to at most seven (7) on the free plan. Silver plan gives 10
day forecast, and the gold and platinum plan increase that limit to 14 days.

### Location specifier

The location can be specified in the same way as in the requests for the current
weather data:

* city name, e.g. `q=London`
* latitude and longitude in degrees, e.g. `q=51.5,12.3`
* post code
  * Canada postal code, e.g. `q=G2J`
  * UK postcode, e.g. `q=SW1`
  * US zip code, e.g. `q=10001`
* IP address
  * automatic IP geolocation lookup: `q=auto:ip`
  * IP address, e.g. `q=123.45.67.89`
* two other codes: metar code and 3 digit airport code

### Example output of forecast data (JSON format) on the free plan

This is an example returned by calls on the free plan. It does not contain an
hour by hour forecast, because that is only available on the paid plans.
Instead, the free plan only contains daily forecast data.

    {
        "location": {
            "name": "Paris",
            "region": "Ile-de-France",
            "country": "France",
            "lat": 48.87,
            "lon": 2.33,
            "tz_id": "Europe/Paris",
            "localtime_epoch": 1533055714,
            "localtime": "2018-07-31 18:48"
        },
        "current": {
            "last_updated_epoch": 1533054609,
            "last_updated": "2018-07-31 18:30",
            "temp_c": 28,
            "temp_f": 82.4,
            "is_day": 1,
            "condition": {
                "text": "Sunny",
                "icon": "//cdn.apixu.com/weather/64x64/day/113.png",
                "code": 1000
            },
            "wind_mph": 12.5,
            "wind_kph": 20.2,
            "wind_degree": 250,
            "wind_dir": "WSW",
            "pressure_mb": 1018,
            "pressure_in": 30.5,
            "precip_mm": 0,
            "precip_in": 0,
            "humidity": 42,
            "cloud": 0,
            "feelslike_c": 28,
            "feelslike_f": 82.4,
            "vis_km": 10,
            "vis_miles": 6
        },
        "forecast": {
            "forecastday": [
                {
                    "date": "2018-07-31",
                    "date_epoch": 1532995200,
                    "day": {
                        "maxtemp_c": 26,
                        "maxtemp_f": 78.8,
                        "mintemp_c": 17,
                        "mintemp_f": 62.6,
                        "avgtemp_c": 22.2,
                        "avgtemp_f": 72,
                        "maxwind_mph": 5.6,
                        "maxwind_kph": 9,
                        "totalprecip_mm": 0.1,
                        "totalprecip_in": 0,
                        "avgvis_km": 10,
                        "avgvis_miles": 6,
                        "avghumidity": 53,
                        "condition": {
                            "text": "Patchy rain possible",
                            "icon": "//cdn.apixu.com/weather/64x64/day/176.png",
                            "code": 1063
                        },
                        "uv": 6.6
                    },
                    "astro": {
                        "sunrise": "06:22 AM",
                        "sunset": "09:31 PM",
                        "moonrise": "11:24 PM",
                        "moonset": "09:49 AM"
                    }
                },
                {
                    "date": "2018-08-01",
                    "date_epoch": 1533081600,
                    "day": {
                        "maxtemp_c": 29.2,
                        "maxtemp_f": 84.6,
                        "mintemp_c": 22.7,
                        "mintemp_f": 72.9,
                        "avgtemp_c": 23.9,
                        "avgtemp_f": 75,
                        "maxwind_mph": 6.9,
                        "maxwind_kph": 11.2,
                        "totalprecip_mm": 0,
                        "totalprecip_in": 0,
                        "avgvis_km": 10,
                        "avgvis_miles": 6,
                        "avghumidity": 52,
                        "condition": {
                            "text": "Partly cloudy",
                            "icon": "//cdn.apixu.com/weather/64x64/day/116.png",
                            "code": 1003
                        },
                        "uv": 7.2
                    },
                    "astro": {
                        "sunrise": "06:23 AM",
                        "sunset": "09:30 PM",
                        "moonrise": "11:47 PM",
                        "moonset": "10:53 AM"
                    }
                },
                ...
            ]
        }
    }

### Relevant forecast information

* forecast.forecastday\[].date: (string) forecast data
* forecast.forecastday\[].date_epoch: (int) Unix timestamp of the forecast date
* forecast.forecastday\[].day.maxtemp_c: (float) maximum temperature for the day
  in °C
* forecast.forecastday\[].day.maxtemp_f: (float) maximum temperature for the day
  in °F
* forecast.forecastday\[].day.mintemp_c: (float) minimum temperature for the day
  in °C
* forecast.forecastday\[].day.mintemp_f: (float) minimum temperature for the day
  in °F
* forecast.forecastday\[].day.totalprecip_mm: (float) total precipation in mm/m²
* forecast.forecastday\[].day.avghumidity: (int) average relative humidity in
  percent
