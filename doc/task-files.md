# weather-information-collector: collection task configuration

Collection tasks are configured via so-called task files.
These files describe what weather information the program shall collect and in
which intervals the collection takes places.
Unlike the [core configuration file](configuration-core.md) these files do not
contain any credentials or similar confidential information, so no additional
precautions have to be applied to them.

# Location of the task files

The location of the collection task files can be specified via the
[core configuration](configuration-core.md). If no location is given there, the
program will look for the task files in these predefined directories, in the
given order:

* `user's home directory`/.wic/task.d
* `user's home directory`/.weather-information-collector/task.d
* /etc/weather-information-collector/task.d
* /etc/wic/task.d
* ./task.d

If none of these directories exist and no location is given in the core
configuration file, then the program will exit with a non-zero exit code.

# Task file elements

## General format

Task settings are basically key-value pairs, where the key is specified first,
followed by an equals sign (`=`), followed by the value of the setting.
For example, the line

    api=OpenWeatherMap

sets the value of `api` to `OpenWeatherMap`.

## Comments

The task files allow comment lines. Comment lines are lines where the first
character is `#`. These lines are ignored by the application. A line has to be
either a comment or task configuration data, you cannot mix both on the same
line.

Completely empty lines are ignored, too.

## Task settings

The following settings are recognized in a task file:

* **api** - name of the API that shall be used during the request; available
  values depend on the currently supported APIs. Currently the following values
  are recognized:
  * **OpenWeatherMap** - data is fetched from OpenWeatherMap
  * **DarkSky** - data is fetched from DarkSky
  * **Weatherbit** - data is fetched from Weatherbit
  * **Weatherstack** - data is fetched from Weatherstack
  * **Apixu** - data is fetched from Apixu (Supported before v0.9.8; now
    deprecated, because Apixu shut down. Do not use anymore.)
* **data** - the type of data that will be collected via the API. If this is not
  set, it will default to `current` for backwards compatibility. Furthermore,
  not every API will support every data type. Currently the following values
  are available:
  * **current** - collect data about the current weather (supported by all
    four APIs)
  * **forecast** - collect weather forecast data (supported by all APIs except
    Weatherstack)
  * **current+forecast** - collect data about current weather and forecast data
    (supported by DarkSky)
* **location.id** - _(optional)_ numeric ID of the location for which the
  weather shall be requested (only supported by OpenWeatherMap)
* **location.name** - name of the location / city, e.g. `London`
* **location.countrycode** - _(optional, except for some Weatherbit requests)_
  ISO 3166 ALPHA-2 country code, e.g. `GB`
* **location.postcode** - _(optional)_ postcode of the location / city; make
  sure that this is a format supported by the corresponding API, or otherwise
  you might get unexpected results / data
* **location.coordinates** - _(optional for all APIs except DarkSky)_
  geographical coordinates (latitude and  longitude) of the location, e.g.
  `51.5,-1.2` - **required for DarkSky tasks**
* **interval** - the interval in seconds after which the next request for the
  same location and the same API will be performed, e.g. `3600`
  (Request intervals below 15 seconds are rejected.)

Although every location-related element itself is optional, each task file must
have _at least one valid location-related setting_ to define the location.
So basically three settings are enough to define a task: api, one
location-related setting and a request interval. The `data` setting will get the
default value `current`, if not set, so it must not be specified in that case.

# Defining multiple tasks

Each task file may contain only information for one single collection task.
However, you can create more than one task file to define several collection
tasks. The number of task files is basically only limited by the number of
requests that the corresponding API allows within a given time frame.
Fortunately, weather-information-collector will check the request limits of all
configured tasks when the task files are loaded. This is to ensure that you do
not exceed the API limit by accident.

# Example of a complete task file

The following example is a complete task file for the
weather-information-collector program (as of version 0.9.0):

    # This line is a comment and will be ignored by the program.
    #And so is this line.

    # use OpenWeatherMap API
    api=OpenWeatherMap
    # collect data about current weather
    data=current
    # city of London, United Kingdom
    location.name=London
    location.countrycode=GB
    location.coordinates=51.507301,-0.1277
    # location's ID for OpenWeatherMap
    location.id=2643743
    # one request every hour, e.g. every 3600 seconds
    interval=3600

Another example would be:

    # use Weatherstack API
    api=Weatherstack
    # collect data of weather forecast
    data=forecast
    # location somewhere in England, near the town of Newbury
    location.coordinates=51.5,-1.2
    # one request two hours minutes, e.g. every 7200 seconds
    interval=7200

An example for the third API would be:

    # use DarkSky API
    api=DarkSky
    # collect current and forecast data
    data=current+forecast
    # location: Alcatraz Island
    location.coordinates=37.8267,-122.4233
    # one request every 20 minutes, e.g. every 1200 seconds
    interval=1200

A possible task file for Weatherbit API could be:

    # use Weatherbit API
    api=Weatherbit
    # collect data about current weather
    data=current
    # city of London, UK
    location.name=London
    location.countrycode=GB
    # one request every two hours, e.g. every 7200 seconds
    interval=7200

Remember that you can create multiple task files to define multiple collection
tasks.
