# weatherbit.io API

## Authentication

Authentication is achieved by passing an API key as request parameter "key",
e.g. https://api.weatherbit.io/v2.0/current?city=Raleigh,NC&key=API-key-here.

## Formats

The API provides all output as JSON.
Other formats (e.g. XML) are not available.

## Error codes

Errors are indicated by returning an error object in JSON and setting a HTTP
status code. Error objects look like this:

    {
        "error": "API key not valid, or not yet activated."
    }

Possible HTTP status codes are:

| HTTP Status Code | Description                                    |
|------------------|------------------------------------------------|
| 200              | No error.                                      |
| 403              | API key is not valid or has not been provided. |

There may be more, but the API documentation does not provide a comprehensive
list of error codes.

## Current weather information

Current weather information can be requested via the
<https://api.weatherbit.io/v2.0/current?key=API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here>
endpoint.

### Location specifier

The location can be specified in several ways:

* city id, e.g. `city_id=1234567`
* city name and country code, e.g. `city=London&country=UK`
* latitude and longitude in degrees, e.g. `lat=51.5&lon=12.3`
* post code and _(optional)_ country, e.g. `postal_code=SW1&country=UK`
* IP address
  * automatic IP geolocation lookup: `ip=auto`
  * IP address, e.g. `ip=123.45.67.89`
* other: ICAO airport code or station code _(premium subscriptions only)_

### Example output (JSON format)

    {
        "data":[
            {
                "rh":42,
                "pod":"d",
                "lon":13.73832,
                "pres":1005.65,
                "timezone":"Europe\/Berlin",
                "ob_time":"2019-03-30 12:37",
                "country_code":"DE",
                "clouds":72,
                "ts":1553949441.8558,
                "solar_rad":459.262,
                "state_code":"13",
                "lat":51.05089,
                "wind_spd":2.00069,
                "wind_cdir_full":"west",
                "wind_cdir":"W",
                "slp":1022.28,
                "vis":24.1,
                "h_angle":12.9,
                "sunset":"17:32",
                "dni":840.12,
                "dewpt":3.8,
                "snow":0,
                "uv":2.26583,
                "precip":0,
                "wind_dir":264,
                "sunrise":"04:43",
                "ghi":608.67,
                "dhi":103.26,
                "aqi":33,
                "city_name":"Dresden",
                "weather":{
                    "icon":"c04d",
                    "code":804,
                    "description":"Overcast clouds"
                },
                "datetime":"2019-03-30:12",
                "temp":16.4,
                "station":"E7277",
                "elev_angle":41.72,
                "app_temp":16.4
            }
        ],
        "count":1
    }

### Relevant weather information

* count: number of returned observations (usually one)
* lat: (float) latitude in degrees
* lon: (float) longitude in degrees
* ob_time: (string) last observation time (YYYY-MM-DD HH:ii)
* ts: (float) last observation time (Unix timestamp, seems like UTC, not local
  timezone)
* datetime: (string) current cycle hour (YYYY-MM-DD:HH)
* timezone: (string) local IANA timezone
* pres: (float) pressure in millibars
* slp: (float) sea level pressure in millibars
* wind_spd: (float) wind speed in m/s
* wind_dir: (int) wind direction in degrees (0=N, 90=E, 180=S, 270=W)
* temp: (float) temperature in °C
* app_temp: (float) apparent temperature ("feels like") in °C
* rh: (int) humidity in percent
* dewpt: (float) dew point in °C
* clouds: (int) cloud coverage as percentage
* vis: (float) visibility in km
* precip: (float) liquid precipitation rate in millimeters per m² per hour
* snow: (float) snowfall in millimeters per m² per hour

## Weather forecast information

Data for hourly weather forecasts can be requested via the
<https://api.weatherbit.io/v2.0/forecast/hourly?key=you-API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here>
endpoint.

The number of hours is limited to at most 48 on non-premium plans. Premium plan
gives up to 120 hour forecasts.

Data for three-hourly weather forecasts can be requested via the
<https://api.weatherbit.io/v2.0/forecast/3hourly?key=you-API-key-here&city=city-name-here&country=ISO-3166-two-letter-code-here>
endpoint.

The three-hourly forecast spans five days.

### Location specifier

The location can be specified in the same way as in the requests for the current
weather data, except for ICAO airport code which is not available:

* city id, e.g. `city_id=1234567`
* city name and country code, e.g. `city=London&country=UK`
* latitude and longitude in degrees, e.g. `lat=51.5&lon=12.3`
* post code and _(optional)_ country, e.g. `postal_code=SW1&country=UK`
* IP address
  * automatic IP geolocation lookup: `ip=auto`
  * IP address, e.g. `ip=123.45.67.89`

### Example output of forecast data

This is an example returned by API calls to the forecast endpoint with an
interval of three hours.

    {
        "data":[
            {
                "wind_cdir":"WSW",
                "rh":60,
                "pod":"n",
                "timestamp_utc":"2019-03-30T18:00:00",
                "pres":1004.25,
                "solar_rad":0,
                "ozone":345.811,
                "weather":{
                    "icon":"c01n",
                    "code":800,
                    "description":"Clear sky"
                },
                "wind_gust_spd":3.09386,
                "timestamp_local":"2019-03-30T19:00:00",
                "snow_depth":0,
                "clouds":1,
                "ts":1553968800,
                "wind_spd":0.959623,
                "pop":0,
                "wind_cdir_full":"west-southwest",
                "slp":1020.99,
                "dni":0,
                "dewpt":5.5,
                "snow":0,
                "uv":0,
                "wind_dir":240,
                "clouds_hi":1,
                "precip":0,
                "vis":24.1,
                "dhi":0,
                "app_temp":12.8,
                "datetime":"2019-03-30:18",
                "temp":12.8,
                "ghi":0,
                "clouds_mid":0,
                "clouds_low":0
            },
            ...
        ],
        "city_name":"Dresden",
        "lon":"13.73832",
        "timezone":"Europe\/Berlin",
        "lat":"51.05089",
        "country_code":"DE",
        "state_code":"13"
    }

### Relevant forecast information

* lat: (float) latitude in degrees
* lon: (float) longitude in degrees
* timezone: (string) local IANA timezone
* data\[].ts: (int) Unix timestamp (seems to be local time, despite the
  documentation saying it's UTC)
* data\[].timestamp_local: (string) timestamp at local time (YYYY-MM-DDTHH:ii:ss)
* data\[].timestamp_utc: (string) timestamp at UTC time (YYYY-MM-DDTHH:ii:ss)
* data\[].wind_spd: (float) wind speed in m/s
* data\[].wind_dir: (int) wind direction in degrees (0=N, 90=E, 180=S, 270=W)
* data\[].temp: (float) temperature in °C
* data\[].app_temp: (float) apparent temperature ("feels like") in °C
* data\[].precip: (float) accumulated liquid precipitation in millimeters per m²
* data\[].snow: (float) accumulated snowfall in millimeters per m²
* data\[].pres: (float) pressure in millibars
* data\[].slp: (float) sea level pressure in millibars
* data\[].rh: (int) humidity in percent
* data\[].dewpt: (float) dew point in °C
* data\[].clouds: (int) cloud coverage as percentage
* data\[].vis: (float) visibility in km
