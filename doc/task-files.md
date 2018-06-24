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
  * **Apixu** - data is fetched from Apixu
  * **DarkSky** - data is fetched from DarkSky
* **location.id** - _(optional)_ numeric ID of the location for which the
  weather shall be requested (only supported by OpenWeatherMap)
* **location.name** - name of the location / city, e.g. `London`
* **location.postcode** - _(optional)_ postcode of the location / city; make
  sure that this is a format supported by the corresponding API, or otherwise
  you might get unexpected results / data
* **location.coordinates** - _(optional for Apixu and OpenWeatherMap)_
  geographical coordinates (latitude and  longitude) of the location, e.g.
  `51.5,-1.2` - **required for DarkSky tasks**
* **interval** - the interval in seconds after which the next request for the
  same location and the same API will be performed, e.g. `3600`
  (Request intervals below 15 seconds are rejected.)

Although every location-related element itself is optional, each task file must
have _at least one valid location-related setting_ to define the location.
So basically three settings are enough to define a task: api, one
location-related setting and a request interval.

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
weather-information-collector program (as of version 0.5.3):

    # This line is a comment and will be ignored by the program.
    #And so is this line.

    # use OpenWeatherMap API
    api=OpenWeatherMap
    # city of London, UK
    location.name=London
    # one request every hour, e.g. every 3600 seconds
    interval=3600

Another example would be:

    # use Apixu API
    api=Apixu
    # location somewhere in England, near the town of Newbury
    location.coordinates=51.5,-1.2
    # one request every 30 minutes, e.g. every 1800 seconds
    interval=1800

An example for the third API would be:

    # use DarkSky API
    api=DarkSky
    # location: Alcatraz Island
    location.coordinates=37.8267,-122.4233
    # one request every 20 minutes, e.g. every 1200 seconds
    interval=1200

Remember that you can create multiple task files to define multiple collection
tasks.
