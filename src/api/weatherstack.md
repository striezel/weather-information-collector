# weatherstack.com API

## Authentication

Authentication is achieved by passing an API key as request parameter
"access_key", e.g. https://api.weatherstack.com/current?key=API-key-here&q=Berlin.

## Formats

The API provides all output as JSON.
Other formats (e.g. XML) are not available.

## Error codes

Errors are indicated by returning an error object in JSON. The HTTP status code
will still be 200 OK in this case, so do not rely on HTTP status codes for error
detection.

Error objects look like this:

    {
        "success": false,
        "error": {
            "code": 101,
            "type": "invalid_access_key",
            "info": "You have not supplied a valid API Access Key. [Technical Support: support@apilayer.com]"
        }
    }

Possible error codes include:


| Error code | Description                                 |
|------------|---------------------------------------------|
| 404        | Request for a non-existing resource.        |
| 101        | API key is invalid or missing.              |
| 102        | API user is blocked / inactive.             |
| 102        | API function does not exist.                |
| 104        | API's rate limit has been reached.          |
| 105        | API function restricted to other plans.     |
| 105        | HTTPS endpoint restricted to other plans.   |

Most notable is probably the fact that HTTPS is _not_ supported on all plans,
so requests have to be done via unencrypted HTTP, including API key. That is
basically a huge design flaw, so let's hope weatherstack.com fixes it soon.

## Current weather information

Current weather information can be requested via the
<http://api.weatherstack.com/current?access_key=API-key-here&query=location-here>
endpoint.

### Location specifier

The location can be specified in several ways:

* city name, e.g. `query=London`
* latitude and longitude in degrees, e.g. `query=51.5,12.3`
* post code
  * Canada postal code, e.g. `query=G2J`
  * UK postcode, e.g. `query=SW1`
  * US zip code, e.g. `query=99501`
* IP address
  * automatic IP geolocation lookup: `query=fetch:ip`
  * IP address, e.g. `query=123.45.67.89`

### Example output (JSON format)

    {
        "request":
        {
            "type":"City",
            "query":"Berlin, Germany",
            "language":"en",
            "unit":"m"
        },
        "location":
        {
            "name":"Berlin",
            "country":"Germany",
            "region":"Berlin",
            "lat":"52.517",
            "lon":"13.400",
            "timezone_id":"Europe\/Berlin",
            "localtime":"2019-10-19 18:55",
            "localtime_epoch":1571511300,
            "utc_offset":"2.0"
        },
        "current":
        {
            "observation_time":"04:55 PM",
            "temperature":14,
            "weather_code":113,
            "weather_icons":
            [
                "https:\/\/assets.weatherstack.com\/images\/wsymbols01_png_64\/wsymbol_0008_clear_sky_night.png"
            ],
            "weather_descriptions":
            [
                "Clear"
            ],
            "wind_speed":17,
            "wind_degree":150,
            "wind_dir":"SSE",
            "pressure":1009,
            "precip":0,
            "humidity":82,
            "cloudcover":0,
            "feelslike":13,
            "uv_index":0,
            "visibility":10,
            "is_day":"no"
        }
    }

### Relevant weather information

Units assume metric units (query parameter `unit=m`).

* location.timezone_id: (string) timezone of the location
* location:localtime: (string) time when the data was updated (YYYY-MM-DD HH:ii)
* location:localtime_epoch: (int) time when the data was updated as UNIX
  timestamp
* current.observation_time: (string) UTC time when data was collected
* current.temperature: (float) temperature in °C
* current.wind_speed: (float) wind speed in km/h
* current.wind_degree: (int) wind direction in degrees (0=N, 90=E, 180=S, 270=W)
* current.wind_dir: (string) wind direction as 16 point compass
* current.pressure: (float) pressure in millibars
* current.precip: (float) precipitation amount in millimeters per m²
* current.humidity: (int) humidity in percent
* current.cloudcover: (int) cloud coverage as percentage
* current.temperature: (float) what the temperature feels like in °C
* current.visibility: (float) visibility in km
* current.weather_descriptions: (array) array of strings describing the current condition

## Weather forecast information

Data for weather forecasts can be requested via the
<http://api.weatherstack.com/forecast?access_key=API-key-here&query=location-here&hourly=1&forecast_days=N>
endpoint.

In that URL, `N` is the number of days for which you want forecasts. The number
of days is limited to at most seven (7) on the professional plan. Business plan
gives up to 14 days forecast.

### Location specifier

The location can be specified in the same way as in the requests for the current
weather data:

* city name, e.g. `query=London`
* latitude and longitude in degrees, e.g. `query=51.5,12.3`
* post code
  * Canada postal code, e.g. `query=G2J`
  * UK postcode, e.g. `query=SW1`
  * US zip code, e.g. `query=99501`
* IP address
  * automatic IP geolocation lookup: `query=fetch:ip`
  * IP address, e.g. `query=123.45.67.89`

### Example output of forecast data (JSON format) on the free plan

__TODO__

### Relevant forecast information

__TODO__
