# OpenWeatherMap API

## Authentication

Authentication is achieved by passing an API key as request parameter "appid",
e.g. http://api.openweathermap.org/data/2.5/weather?id=12345&appid=API-key-here.

## Formats

The API provides two formats, JSON and XML. JSON is the default, but it can be
changed to XML by providing an appropriate `mode` parameter:

* JSON: http://api.openweathermap.org/data/2.5/weather?id=2643743&appid=API-key-here
* XML:  http://api.openweathermap.org/data/2.5/weather?id=2643743&appid=API-key-here&mode=xml

## Error codes

Errors are indicated by returning an error object in JSON and setting a HTTP
status code. Error objects look like this:

    {
        "cod": 401,
        "message": "Invalid API key. Please see http://openweathermap.org/faq#error401 for more info."
    }

The `cod` element usually contains the HTTP status code.

## Current weather information

Curent weather information can be requested via the
<http://api.openweathermap.org/data/2.5/weather?id=city-id-here&appid=API-key-here>
endpoint.

### Location specifier

The location can be specified in several ways:

* city name, e.g. `q=London` or `q=London,UK`
  (http://api.openweathermap.org/data/2.5/weather?q=London&appid=API-key-here)
* by city ID (preferred way), e.g. `id=2643743`
  (http://api.openweathermap.org/data/2.5/weather?id=2643743&appid=API-key-here)
* latitude and longitude in degrees, e.g. `lat=51.5&lon=-0.13`
  (http://api.openweathermap.org/data/2.5/weather?lon=-0.13&lat=51.51&appid=API-key-here)
* zip code, e.g. `zip=90210,us`
  (http://api.openweathermap.org/data/2.5/weather?zip=90210,US&appid=API-key-here)
  * Note: If no country code is given, it defaults to USA.

### Example output (JSON format)

    {
        "coord":
        {
            "lon": -0.13,
            "lat": 51.51
        },
        "weather":
        [
            {
                "id": 701,
                "main": "Mist",
                "description": "mist",
                "icon": "50n"
            }
        ],
        "base": "stations",
        "main":
        {
            "temp": 285.9,
            "pressure": 1012,
            "humidity": 87,
            "temp_min": 285.15,
            "temp_max": 287.15
        },
        "visibility": 10000,
        "wind":
        {
            "speed": 4.1,
            "deg": 260
        },
        "clouds":
        {
            "all": 75
        },
        "dt": 1502243400,
        "sys":
        {
            "type": 1,
            "id": 5093,
            "message": 0.0024,
            "country": "GB",
            "sunrise": 1502253409,
            "sunset": 1502307244
        },
        "id": 2643743,
        "name": "London",
        "cod": 200
    }

### Relevant weather information

* id: (int) city identification
* dt: (int) data receiving time as Unix timestamp (UTC)
* sys.sunrise: (int) sunrise time as Unix timestamp (UTC)
* sys.sunset: (int) sunset time as Unix timestamp (UTC)
* main.temp: (float) temperature in Kelvin
* main.humidity: (int) humidity in percent
* main.temp_min: (float) minimum temperature at the moment in Kelvin. This is a
  deviation from the current temperature for larger cities. The parameter might
  or might not be present, i.e. do not rely on it being there.
* main.temp_max: (float) maximum temperature at the moment in Kelvin. This is a
  deviation from the current temperature for larger cities. The parameter might
  or might not be present, i.e. do not rely on it being there.
* main.pressure: (int?) pressure in hPa (on the sea level, if there is no
  sea_level or grnd_level data)
  * Note: 1 hPa = 1 mbar
* main.sea_level: (int?) pressure on the sea level in hPa (element is optional)
* main.grnd_level: (int?) pressure on the ground level in hPa (element is
  optional)
* wind.speed: (float) wind speed in m/s
* wind.deg: (int) wind direction in degrees (0=N, 90=E, 180=S, 270=W)
* clouds.all: (int) cloudiness in percent
* rain.3h: (int) precipation volume for the last 3 hours in mm/m² (optional)
* snow.3h: (int) snow volume for the last 3 hours in mm/m² (optional)
* weather\[].id: (int) weather condition ID
  * See <http://openweathermap.org/weather-conditions> for more information on
    weather conditions.
  * It is possible for a location to meet more than one weather condition.
* weather\[].main: (string) group of weather condition
* weather\[].description: (string) description of weather condition
