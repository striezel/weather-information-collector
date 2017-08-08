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

Curent weather information can be requested via the
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
* two other codes: metar code ad 3 digit airport code

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
