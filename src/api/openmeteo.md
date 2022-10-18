# Open-Meteo API

## Authentication

The API requires no authentication, i. e. no API key needs to be sent as part of
the requests.

## Formats

The API provides its data as JSON.

## Error codes

Errors are indicated by returning an error object in JSON and setting a HTTP
status code to 400 (=Bad Request). Error objects look like this:

    {
        "reason": "Cannot initialize IconSurfaceVariable from invalid String value winddiren_10m for key hourly",
        "error": true
    }

## Current weather information

Current weather information can be requested by adding `current_weather=true` as
query parameter to a request. For example, the following URL would request the
current weather data for Berlin, Germany:
<https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current_weather=true>

### Location specifier

The location can be specified by giving latitude and longitude of the location
in degrees as part of the request URL, e.g.:

    https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current_weather=true

### Example output

```json
{
  "latitude": 52.52,
  "longitude": 13.419998,
  "generationtime_ms": 0.3770589828491211,
  "utc_offset_seconds": 0,
  "timezone": "GMT",
  "timezone_abbreviation": "GMT",
  "elevation": 38,
  "current_weather": {
    "temperature": 16.8,
    "windspeed": 8.2,
    "winddirection": 105,
    "weathercode": 61,
    "time": "2022-09-08T13:00"
  }
}
```

### Relevant weather information

* current_weather.temperature: (float) temperature in ° Celsius
* current_weather.windspeed: (float) wind speed in km/h
* current_weather.winddirection: (float) wind direction in degrees,
  with 0=N, 90=E, 180=S, 270=W
* current_weather.weathercode: (int) numeric code that indicates the current
  weather condition

Weather codes are:

| Code       | Description                                      |
|------------|--------------------------------------------------|
| 0          | Clear sky                                        |
| 1, 2, 3    | Mainly clear, partly cloudy, and overcast        |
| 45, 48     | Fog and depositing rime fog                      |
| 51, 53, 55 | Drizzle: Light, moderate, and dense intensity    |
| 56, 57     | Freezing Drizzle: Light and dense intensity      |
| 61, 63, 65 | Rain: Slight, moderate and heavy intensity       |
| 66, 67     | Freezing Rain: Light and heavy intensity         |
| 71, 73, 75 | Snow fall: Slight, moderate, and heavy intensity |
| 77         | Snow grains                                      |
| 80, 81, 82 | Rain showers: Slight, moderate, and violent      |
| 85, 86     | Snow showers slight and heavy                    |
| 95         | Thunderstorm: Slight or moderate                 |
| 96, 99     | Thunderstorm with slight and heavy hail          |

## Weather forecast information

Data for weather forecasts can be requested via the
<https://api.open-meteo.com/v1/forecast?latitude=latitude-here&longitude=longitude-here&hourly=temperature_2m,relativehumidity_2m,precipitation,rain,showers,snowfall,pressure_msl,surface_pressure,cloudcover,windspeed_10m,winddirection_10m&windspeed_unit=ms>
endpoint.

### Location specifier

The location can be specified by giving latitude and longitude of the location
in degrees as part of the request URL, e.g.:

    https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,relativehumidity_2m,precipitation,rain,showers,snowfall,pressure_msl,surface_pressure,cloudcover,windspeed_10m,winddirection_10m&windspeed_unit=ms

### Example output of forecast data

```json
{
  "latitude": 52.52,
  "longitude": 13.419998,
  "generationtime_ms": 1.1960268020629883,
  "utc_offset_seconds": 0,
  "timezone": "GMT",
  "timezone_abbreviation": "GMT",
  "elevation": 38,
  "hourly_units": {
    "time": "iso8601",
    "temperature_2m": "°C",
    "relativehumidity_2m": "%",
    "precipitation": "mm",
    "rain": "mm",
    "showers": "mm",
    "snowfall": "cm",
    "pressure_msl": "hPa",
    "surface_pressure": "hPa",
    "cloudcover": "%",
    "windspeed_10m": "m/s",
    "winddirection_10m": "°"
  },
  "hourly": {
    "time": [
      "2022-09-08T00:00",
      "2022-09-08T01:00",
      "2022-09-08T02:00",
      "2022-09-08T03:00",
      "2022-09-08T04:00"
    ],
    "temperature_2m": [
      17.7,
      17.1,
      16.6,
      16.2,
      15.8
    ],
    "relativehumidity_2m": [
      61,
      61,
      62,
      61,
      60
    ],
    "precipitation": [
      0,
      0.3,
      0.1,
      1.1,
      2.6
    ],
    "rain": [
      0,
      0.1,
      1.1,
      0.5,
      1.3
    ],
    "snowfall": [
      0,
      0,
      0,
      0,
      0
    ],
    "pressure_msl": [
      1012.1,
      1011.5,
      1011.3,
      1010.2,
      1010.7
    ],
    "surface_pressure": [
      1007.6,
      1007,
      1006.8,
      1005.7,
      1006.2
    ],
    "cloudcover": [
      100,
      99,
      93,
      95,
      1
    ],
    "windspeed_10m": [
      2.62,
      2.9,
      2.4,
      2.55,
      2.72
    ],
    "winddirection_10m": [
      108,
      90,
      88,
      79,
      84
    ]
  }
}
```

### Relevant forecast information

* timezone: (string) timezone identifier (e. g. "Europe/Athens")
* timezone_abbreviation: (string) timezone abbreviation (e. g. "EET")
* utc_offset_seconds: (int) timezone offset in seconds

Weather data is represented as arrays in the `hourly` JSON object. Each member
of the `hourly` object is an array of the same length. Elements with the same
array index belong together, i. e. `hourly.time[0]` is the time for the first
forecast element, and `hourly.temperature_2m[0]` is the corresponding
temperature value, and `hourly.relative_humidity_2m[0]` is the humidity value
for that data point, and so on.

The following arrays are all part of the `hourly` object:

* hourly.time\[]: (string) time of the forecasted data as string,
  e. g. "2022-09-08T12:34"
* hourly.temperature_2m\[]: (float) temperature at 2 meters above the ground in
  ° C
* hourly.relative_humidity_2m\[]: (int) relative humidity at 2 meters above
  the ground
* hourly.precipitation\[]: (float) total precipitation sum of the preceding hour
  in millimeters
* hourly.rain\[]: (float) rain from large scale weather systems of the preceding
  hour in millimeters
* hourly.showers\[]: (float) showers from convective precipitation of the
  preceding hour in millimeters
* hourly.snowfall\[]: (float) snow amount of the preceding hour in centimeters;
  for the water equivalent in centimeters divide by seven, e. g. 7 cm snow equal
  10 mm precipitation water
* hourly.pressure_msl\[]: (float) pressure in hPa, reduced to mean sea level
* hourly.surface_pressure\[]: (float) pressure in hPa at surface
* hourly.cloudcover\[]: (int) cloudiness in percent
* hourly.windspeed_10m\[]: (float) wind speed in m/s (or the specified windspeed
  unit from `hourly_units.windspeed_10m`)
* hourly.winddirection_10m\[]: (int) wind direction in degrees (0=N, 90=E, 180=S, 270=W)

Depending on the value of the request parameter `&hourly=...`, some of those
items may or may not be present.
