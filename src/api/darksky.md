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

Curent weather information can be requested via the
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
* currently.temperature: (float) text current temperature in °C
* currently.apparentTemperature: (float) what the temperature feels like, in °C
* currently.dewPoint: (float) the dew point in °C
* currently.humidity: (float) relative humidity, within [0;1]
* currently.pressure: (float) pressure in hPa
* currently.windSpeed: (float) wind speed in meters per second
* currently.windBearing: (float) direction where the wind is coming from in
  degrees, with 0=N,90=E,180=S,270=W 
* currently.cloudCover: (float) ratio of sky occluded by clouds, within [0;1]
* currently.visibility: (float) visibility in km

