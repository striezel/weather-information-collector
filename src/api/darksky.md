# darksky.net API

## Authentication

Authentication is achieved by passing an API key as part of the URL,
e.g. https://api.darksky.net/forecast/API-key-here/[latitude],[longitude].

## Formats

The API provides all output as JSON.
Other formats (e.g. XML) are not available.

## Error codes

Errors are indicated by returning an object with error message and status code.
Additionally, an HTTP status code is set, too. Error objects look like this:

    {
        "code": 400,
        "error": "The given location is invalid."
    }

Possible HTTP status codes and errors include:


| HTTP Status Code | Description                           |
|------------------|---------------------------------------|
| 400              | Latitude and/or longitude is invalid. |
| 403              | The API key is invalid or incorrect.  |
| 404              | API key is empty.                     |

The table is not exhaustive, there may be more (e.g. for reaching a rate limit
on the free plan).

## Current weather information

Current weather information can be requested via the
<https://api.darksky.net/forecast/API-key-here/[latitude],[longitude].>
endpoint.

### Location specifier

The location can be specified by giving latitude and longitude of the location
in degrees as part of the request URL, e.g.:

    https://api.darksky.net/forecast/API-key-here/51.5,12.3

### Specification of units for response

The units of a response can be set via the query parameter `units`. For example,
the request to get the response in SI units would be

    https://api.darksky.net/forecast/API-key-here/51.5,12.3?units=si

Allowed values for the units parameter are:

* **auto**: automatically select units based on geographic location
* **ca**: same as `si`, except that `windSpeed` and `windGust` are in kilometers per hour
* **uk2**: same as `si`, except that `nearestStormDistance` and `visibility`
  are in miles, and `windSpeed` and `windGust` in miles per hour
* **us**: Imperial units - this is the default, if no units parameter is given!
* **si**: SI units

SI units are as follows:

* **summary**: Any summaries containing temperature or snow accumulation units
  will have their values in degrees Celsius or in centimeters (respectively).
* **nearestStormDistance**: kilometers
* **precipIntensity**: millimeters per hour
* **precipIntensityMax**: millimeters per hour
* **precipAccumulation**: centimeters
* **temperature**: degrees Celsius
* **temperatureMin**: degrees Celsius
* **temperatureMax**: degrees Celsius
* **apparentTemperature**: degrees Celsius
* **dewPoint**: degrees Celsius
* **windSpeed**: meters per second
* **windGust**: meters per second
* **pressure**: hectopascals
* **visibility**: kilometers

### Example output (JSON format, shortened)

    {
        "latitude": 37.8267,
        "longitude": -122.4233,
        "timezone": "America/Los_Angeles",
        "currently":
        {
            "time": 1529851404,
            "summary": "Clear",
            "icon": "clear-day",
            "nearestStormDistance": 399,
            "nearestStormBearing": 154,
            "precipIntensity": 0,
            "precipProbability": 0,
            "temperature": 15.15,
            "apparentTemperature": 15.15,
            "dewPoint": 12.33,
            "humidity": 0.83,
            "pressure": 1009.82,
            "windSpeed": 2.46,
            "windGust": 4.56,
            "windBearing": 245,
            "cloudCover": 0.14,
            "uvIndex": 1,
            "visibility": 15.98,
            "ozone": 300.84
        },
        "minutely":
        {
            "summary": "Clear for the hour.",
            "icon": "clear-day",
            "data": [
                {
                    "time": 1529851380,
                    "precipIntensity": 0,
                    "precipProbability": 0
                },
                {
                    "time": 1529851440,
                    "precipIntensity": 0,
                    "precipProbability": 0
                },
                ...
            ]
        },
        "hourly":
        {
            "summary": "Mostly cloudy starting this evening.",
            "icon": "partly-cloudy-night",
            "data": [
                {
                    "time": 1529848800,
                    "summary": "Clear",
                    "icon":"clear-day",
                    "precipIntensity": 0,
                    "precipProbability": 0,
                    "temperature": 14.79,
                    "apparentTemperature": 14.79,
                    "dewPoint": 12.26,
                    "humidity": 0.85,
                    "pressure": 1009.57,
                    "windSpeed": 2.42,
                    "windGust": 3.98,
                    "windBearing": 253,
                    "cloudCover": 0.1,
                    "uvIndex": 0,
                    "visibility": 15.55,
                    "ozone": 300.65
                },
                ...
            ]
        },
        "daily":
        {
            "summary": "No precipitation throughout the week, with high temperatures bottoming out at 17°C on Wednesday.",
            "icon": "clear-day",
            "data": [
                {
                    "time": 1529823600,
                    "summary": "Partly cloudy starting in the evening.",
                    "icon": "partly-cloudy-night",
                    "sunriseTime": 1529844603,
                    "sunsetTime": 1529897814,
                    "moonPhase": 0.4,
                    "precipIntensity": 0.0076,
                    "precipIntensityMax": 0.0406,
                    "precipIntensityMaxTime": 1529859600,
                    "precipProbability": 0.05,
                    "precipType": "rain",
                    "temperatureHigh": 18.87,
                    "temperatureHighTime": 1529874000,
                    "temperatureLow": 12.3,
                    "temperatureLowTime": 1529928000,
                    "apparentTemperatureHigh": 18.87,
                    "apparentTemperatureHighTime": 1529874000,
                    "apparentTemperatureLow": 12.3,
                    "apparentTemperatureLowTime": 1529928000,
                    "dewPoint": 12.17,
                    "humidity": 0.78,
                    "pressure": 1010.27,
                    "windSpeed": 3.66,
                    "windGust": 7.81,
                    "windGustTime": 1529874000,
                    "windBearing": 241,
                    "cloudCover": 0.2,
                    "uvIndex": 10,
                    "uvIndexTime": 1529870400,
                    "visibility": 16.09,
                    "ozone": 300.77,
                    "temperatureMin": 13.87,
                    "temperatureMinTime": 1529906400,
                    "temperatureMax": 18.87,
                    "temperatureMaxTime": 1529874000,
                    "apparentTemperatureMin": 13.87,
                    "apparentTemperatureMinTime": 1529906400,
                    "apparentTemperatureMax": 18.87,
                    "apparentTemperatureMaxTime": 1529874000
                },
                ...
            ]
        }
    }

The data objects in `minutely` are repeated once for every minute within the
next 60 minutes. Data in `hourly` covers the next 48 hours, and the data
provided in `daily` covers the next seven days. To shorten the data a bit, only
one or two elements of these arrays are shown in the example above.

### Relevant weather information

All the relevant information for the current weather is present in the object
`currently` right under the JSON root. That is why only those elements will be
listed below. Furthermore, we assume that `units=si` has been passed as part of
the query string, i.e. all values use SI units.

* currently.time: (int) time when the data was updated as Unix timestamp
* currently.precipIntensity: (float) precipitation occurring at the given time
  in millimeters per m² and hour
* currently.precipProbability: (float) probability of precipitation occurring,
  within [0;1]
* currently.temperature: (float) current temperature in °C
* currently.apparentTemperature: (float) what the temperature feels like, in °C
* currently.dewPoint: (float) the dew point in °C
* currently.humidity: (float) relative humidity, within [0;1]
* currently.pressure: (float) pressure in hPa
* currently.windSpeed: (float) wind speed in meters per second
* currently.windBearing: (float) direction where the wind is coming from in
  degrees, with 0=N,90=E,180=S,270=W 
* currently.cloudCover: (float) ratio of sky occluded by clouds, within [0;1]
* currently.visibility: (float) visibility in km

## Weather forecast information

Data for weater forecasts can be requested via the
<https://api.darksky.net/forecast/API-key-here/[latitude],[longitude].>
endpoint. (Yes, that is the same as for the current weather.)

### Location specifier

The location can be specified by giving latitude and longitude of the location
in degrees as part of the request URL, e.g.:

    https://api.darksky.net/forecast/API-key-here/51.5,12.3

### Specification of units for response

These are the same as for the current weather information. See the paragraph
about the current weather information above for details.

### Example output of forecast data (JSON format, shortened)

    {
        "latitude": 52.516,
        "longitude": 13.378,
        "timezone": "Europe/Berlin",
        "currently": {
            "time": 1533403488,
            "summary": "Partly Cloudy",
            "icon": "partly-cloudy-day",
            "precipIntensity": 0.061,
            "precipProbability": 0.02,
            "precipType": "rain",
            "temperature": 30.49,
            "apparentTemperature": 30.49,
            "dewPoint": 15.21,
            "humidity": 0.4,
            "pressure": 1015.35,
            "windSpeed": 2.8,
            "windGust": 4.63,
            "windBearing": 329,
            "cloudCover": 0.33,
            "uvIndex": 0,
            "visibility": 14.48,
            "ozone": 290.79
        },
        "hourly": {
            "summary": "Partly cloudy until tomorrow evening.",
            "icon": "partly-cloudy-day",
            "data": [
                {
                    "time": 1533402000,
                    "summary": "Partly Cloudy",
                    "icon": "partly-cloudy-day",
                    "precipIntensity": 0.0686,
                    "precipProbability": 0.02,
                    "precipType": "rain",
                    "temperature": 31.18,
                    "apparentTemperature": 31.18,
                    "dewPoint": 14.78,
                    "humidity": 0.37,
                    "pressure": 1015.28,
                    "windSpeed": 3.1,
                    "windGust": 4.37,
                    "windBearing": 320,
                    "cloudCover": 0.34,
                    "uvIndex": 1,
                    "visibility": 12.67,
                    "ozone": 290.85
                },
                {
                    "time": 1533405600,
                    "summary": "Partly Cloudy",
                    "icon": "partly-cloudy-day",
                    "precipIntensity": 0.0508,
                    "precipProbability": 0.02,
                    "precipType": "rain",
                    "temperature": 29.51,
                    "apparentTemperature": 29.51,
                    "dewPoint": 15.69,
                    "humidity": 0.43,
                    "pressure": 1015.46,
                    "windSpeed": 2.53,
                    "windGust": 4.99,
                    "windBearing": 345,
                    "cloudCover": 0.3,
                    "uvIndex": 0,
                    "visibility": 16.09,
                    "ozone": 290.69
                },
                ...
                {
                    "time": 1533574800,
                    "summary": "Clear",
                    "icon": "clear-day",
                    "precipIntensity": 0,
                    "precipProbability": 0,
                    "temperature": 28.3,
                    "apparentTemperature": 28.3,
                    "dewPoint": 8.79,
                    "humidity": 0.29,
                    "pressure": 1014.95,
                    "windSpeed": 1.19,
                    "windGust": 1.63,
                    "windBearing": 194,
                    "cloudCover": 0,
                    "uvIndex": 1,
                    "visibility": 16.09,
                    "ozone": 306.61
                }
            ]
        },
        "daily": {
            "summary": "Rain on Thursday, with high temperatures peaking at 35°C on Wednesday.",
            "icon": "rain",
            "data": [
                {
                    "time": 1533333600,
                    "summary": "Partly cloudy starting in the afternoon.",
                    "icon": "partly-cloudy-day",
                    "sunriseTime": 1533353543,
                    "sunsetTime": 1533408922,
                    "moonPhase": 0.74,
                    "precipIntensity": 0.0254,
                    "precipIntensityMax": 0.1118,
                    "precipIntensityMaxTime": 1533416400,
                    "precipProbability": 0.08,
                    "precipType": "rain",
                    "temperatureHigh": 33.02,
                    "temperatureHighTime": 1533387600,
                    "temperatureLow": 20.03,
                    "temperatureLowTime": 1533445200,
                    "apparentTemperatureHigh": 33.02,
                    "apparentTemperatureHighTime": 1533387600,
                    "apparentTemperatureLow": 20.04,
                    "apparentTemperatureLowTime": 1533445200,
                    "dewPoint": 13.31,
                    "humidity": 0.44,
                    "pressure": 1016.96,
                    "windSpeed": 0.37,
                    "windGust": 7.93,
                    "windGustTime": 1533416400,
                    "windBearing": 13,
                    "cloudCover": 0.23,
                    "uvIndex": 5,
                    "uvIndexTime": 1533380400,
                    "visibility": 11.8,
                    "ozone": 288.13,
                    "temperatureMin": 21.07,
                    "temperatureMinTime": 1533351600,
                    "temperatureMax": 33.02,
                    "temperatureMaxTime": 1533387600,
                    "apparentTemperatureMin": 21.07,
                    "apparentTemperatureMinTime": 1533351600,
                    "apparentTemperatureMax": 33.02,
                    "apparentTemperatureMaxTime": 1533387600
                },
                {
                    "time": 1533420000,
                    "summary": "Partly cloudy throughout the day.",
                    "icon": "partly-cloudy-day",
                    "sunriseTime": 1533440040,
                    "sunsetTime": 1533495215,
                    "moonPhase": 0.78,
                    "precipIntensity": 0.0102,
                    "precipIntensityMax": 0.0914,
                    "precipIntensityMaxTime": 1533420000,
                    "precipProbability": 0.11,
                    "precipType": "rain",
                    "temperatureHigh": 25.34,
                    "temperatureHighTime": 1533484800,
                    "temperatureLow": 15.46,
                    "temperatureLowTime": 1533528000,
                    "apparentTemperatureHigh": 25.34,
                    "apparentTemperatureHighTime": 1533484800,
                    "apparentTemperatureLow": 15.46,
                    "apparentTemperatureLowTime": 1533528000,
                    "dewPoint": 13.01,
                    "humidity": 0.56,
                    "pressure": 1018.45,
                    "windSpeed": 3.56,
                    "windGust": 7.26,
                    "windGustTime": 1533420000,
                    "windBearing": 320,
                    "cloudCover": 0.47,
                    "uvIndex": 4,
                    "uvIndexTime": 1533463200,
                    "visibility": 16.09,
                    "ozone": 301.58,
                    "temperatureMin": 20.03,
                    "temperatureMinTime": 1533445200,
                    "temperatureMax": 25.34,
                    "temperatureMaxTime": 1533484800,
                    "apparentTemperatureMin": 20.04,
                    "apparentTemperatureMinTime": 1533445200,
                    "apparentTemperatureMax": 25.34,
                    "apparentTemperatureMaxTime": 1533484800
                },
                ...
                {
                    "time": 1533938400,
                    "summary": "Partly cloudy overnight.",
                    "icon": "partly-cloudy-night",
                    "sunriseTime": 1533959032,
                    "sunsetTime": 1534012937,
                    "moonPhase": 0,
                    "precipIntensity": 0,
                    "precipIntensityMax": 0,
                    "precipProbability": 0,
                    "temperatureHigh": 24.73,
                    "temperatureHighTime": 1534003200,
                    "temperatureLow": 15.18,
                    "temperatureLowTime": 1534042800,
                    "apparentTemperatureHigh": 24.73,
                    "apparentTemperatureHighTime": 1534003200,
                    "apparentTemperatureLow": 15.18,
                    "apparentTemperatureLowTime": 1534042800,
                    "dewPoint": 7.29,
                    "humidity": 0.46,
                    "pressure": 1023.1,
                    "windSpeed": 1.86,
                    "windGust": 9,
                    "windGustTime": 1533938400,
                    "windBearing": 247,
                    "cloudCover": 0.04,
                    "uvIndex": 5,
                    "uvIndexTime": 1533981600,
                    "visibility": 16.09,
                    "ozone": 331.1,
                    "temperatureMin": 13.58,
                    "temperatureMinTime": 1533956400,
                    "temperatureMax": 24.73,
                    "temperatureMaxTime": 1534003200,
                    "apparentTemperatureMin": 13.58,
                    "apparentTemperatureMinTime": 1533956400,
                    "apparentTemperatureMax": 24.73,
                    "apparentTemperatureMaxTime": 1534003200
                }
            ]
        },
        "flags": {
            "sources": [
                "dwdpa",
                "cmc",
                "gfs",
                "icon",
                "isd",
                "madis"
            ],
            "nearest-station": 5.095,
            "units": "si"
        },
        "offset": 2
    }

### Relevant forecast information

The element `hourly` contains the relevant information. Other elements (e.g.
`currently` or `daily`) can be ignored. Furthermore, we assume that `units=si`
has been passed as part of the query string, i.e. all values use SI units.

* hourly.data\[].time: (int) time of forecasted weather condition as Unix
  timestamp according to the local time zone
* hourly.data\[].temperature: (float) predicted temperature in °C
* hourly.data\[].apparentTemperature: (float) what the predicted temperature
  feels like, in °C
* hourly.data\[].precipIntensity: (float) precipitation occurring at the given
  time in millimeters per square meter and hour
* hourly.data\[].precipProbability: (float) probability of precipitation
  occurring, within [0;1]
* hourly.data\[].humidity: (float) relative humidity, within [0;1]
* hourly.data\[].pressure: (float) pressure in hPa
* hourly.data\[].windSpeed: (float) wind speed in meters per seconds
* hourly.data\[].windBearing: (int / float?) direction where the wind is coming
  from in degrees, with 0=N,90=E,180=S,270=W
* hourly.data\[].cloudCover: (float) ratio of sky occluded by clouds,
  within [0;1]
* hourly.data\[].visibility: (float) visibility in km
