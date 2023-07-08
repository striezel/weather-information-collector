# Version history of weather-information-collector

## Version 0.?.? (2023-04-??)

simdjson, the library used for JSON parsing, has been updated from version 3.1.6
to version 3.2.1.

## Version 0.11.1 (2023-04-01)

This release fixes a problem where current weather data gathered from Open-Meteo
API could not be saved to the database.

## Version 0.11.0 (2023-04-01)

Tasks using the DarkSky API are not allowed anymore and will cause an error when
trying to run the weather-information-collector. Since DarkSky has been shut
down by Apple on 31st March 2023, this is just the right course of action.

simdjson, the library used for JSON parsing, has been updated from version 3.0.1
to version 3.1.6.

## Version 0.10.1 (2023-01-01)

An informational notice will be shown when there is at least one task uses the
DarkSky API to get data. DarkSky will shut down on 31st March 2023, so users
get a notice to prepare for migration to other APIs.

The request limit for Weatherbit's free plan is adjusted to 50 requests per day,
since they changed the limit from 500 to 50 requests.

Furthermore, support for Weatherbit's Hobbyist plan (350 requests per day) is
added.

simdjson, the library used for JSON parsing, has been updated from version 3.0.0
to version 3.0.1.

## Version 0.10.0 (2022-10-24)

The program is now able to collect data from the Open-Meteo API, too.

weather-information-collector-task-creator can now work even if there is no
API key for OpenWeatherMap available in the configuration file. It will fall
back to Open-Meteo in that case.

simdjson, the library used for JSON parsing, has been updated from version 2.2.2
to version 3.0.0.

nlohmann/json, the library used for JSON parsing when simdjson is not supported,
has been updated from version 3.11.1 to version 3.11.2.

## Version 0.9.22 (2022-08-18)

simdjson, the library used for JSON parsing, has been updated from version 1.0.0
to version 2.2.2.

nlohmann/json, the library used for JSON parsing when simdjson is not supported,
has been updated from version 3.10.2 to version 3.11.1.

weather-information-collector can now be
[built on Windows](./doc/msys2-build.md) with the help of
[MSYS2](https://www.msys2.org/).

## Version 0.9.21 (2021-10-05)

A possible null-pointer dereference in the update program has been fixed.

simdjson, the library used for JSON parsing, has been updated from version 0.7.1
to version 1.0.0.

nlohmann/json, the library used for JSON parsing when simdjson is not supported,
has been updated from version 3.9.1 to version 3.10.2.

## Version 0.9.20 (2021-08-24)

weather-information-collector and weather-information-collector-task-creator do
now show the version of the cURL library as part of their version information,
too.

The collector, synchronization and update program do now show the version of the
MariaDB client library as part of their version information, too.

## Version 0.9.19 (2021-06-14)

simdjson, the library used for JSON parsing, has been updated from version 0.6.0
to version 0.7.1.

nlohmann/json, the library used for JSON parsing when simdjson is not supported,
has been updated from version 3.5.0 to version 3.9.1.

## Version 0.9.18 (2021-04-28)

The minimum required CMake version for compiling the project is raised from 2.8
to 3.8.

## Version 0.9.17 (2021-04-26)

Precision of temperature values is now limited to three decimal digits after
the decimal separator, i. e. values like 23.4560001 °C will become 23.456 °C
instead. The update program will adjust the values accordingly when executed.

## Version 0.9.16 (2021-04-23)

- Parser errors will now show more information why JSON parsing failed.
- A potential error with finding the user's home directory has been fixed.
- HTTPS connections will now try to use TLS v1.2 or newer by default, if your
  system uses cURL 7.54.0 or newer. For older cURL versions, the minimum TLS
  version is 1.0 as it was before.

## Version 0.9.15 (2020-11-11)

simdjson, the library used for JSON parsing, has been updated from version 0.3.1
to version 0.6.0.

## Version 0.9.14 (2020-09-20)

Restore capabilities of the update program to update from some very old
versions. In particular, updates from version older than 0.8.3 are now possible
again.

## Version 0.9.13 (2020-05-03)

weather-information-collector will now use simdjson for JSON parsing, if the
compiler supports that. Otherwise it will fall back to nlohmann/json for the
parsing of JSON content.

Furthermore, all remaining uses of the JsonCpp library have been removed.

## Version 0.9.12 (2020-04-28)

The C++ standard used during compilation has been raised from C++14 to C++17.
Furthermore, the Boost filesystem library has been replaced by `std::filesystem`
which is part of the standard library of C++17.
Recent compilers should support that by now.

## Version 0.9.11 (2020-04-19)

- The library for database requests changes from libmysql++ to libmariadb.
  libmariadb is available on more of the common Linux distributions than
  libmysql++, so the weather-information-collector can also be built and run on
  more distributions.
- **Backwards-incompatible change:** The update program will no longer support
  updating from versions older than 0.8.3. If you need this functionality, then
  use v0.9.10 to perform the updates before moving on to v0.9.11 or newer.

  **Note: This backwards-incompatibility was fixed later in version 0.9.14,
  i.e. the update program is again fully functional in version 0.9.14.**

## Version 0.9.10 (2020-03-07)

- Weatherbit forecast will now use daily forecast for free plan and hourly
  forecast for other plans.
- Perform Weatherstack requests via HTTPS when the configured plan allows it.
  (Non-free plans allow HTTPS requests, the free plan only supports HTTP.)

## Version 0.9.9 (2020-03-01)

It is now possible to specify which plans are used for the APIs of
OpenWeatherMap, Weatherbit and Weatherstack. Checks for API rate limits will
respect those plans.

## Version 0.9.8 (2019-10-26)

Tasks using the Apixu API are not allowed anymore and will cause an error when
trying to run the weather-information-collector. Since Apixu has been shut
down by its owner, this is just the right course of action.

## Version 0.9.7 (2019-10-22)

The program is now able to collect data from the Weatherstack API, too.
However, only data about the current weather can be collected. Forecast is only
available on the professional plan or higher plans, and therefore it cannot be
tested and implemented.

Furthermore, a deprecation notice will be shown whenever a task uses the Apixu
API to get data. That API has been shut down, so no more data can be retrieved
from it.

## Version 0.9.6 (2019-09-22)

Synchronization program will not start the synchronization, if both the source
and the destination configuration point to the same database.

## Version 0.9.5 (2019-07-13)

Fix build error with benchmark (not build by default, only by CI) and add a
Dockerfile as alternative to native build.

## Version 0.9.4 (2019-06-30)

The synchronization program will now cause less traffic to the destination
server which potentially speeds up the synchronization, especially if the
network connection to the destination server is slow.

## Version 0.9.3 (2019-06-30)

Default batch size of the synchronization program is reduced from 125 to 40.

## Version 0.9.2 (2019-05-09)

Switch Weatherbit.io forecast from 3-hourly / 5 day forecast to hourly / 2 day
forecast, because the first is only available for users on premium plans.

## Version 0.9.1 (2019-05-05)

The collector and synchronization programs will now check whether the databases
are up to date before performing their tasks. That way data corruption can be
avoided when someone accidentally runs one of these programs with an older
database.

To disable that check and return to the behaviour as of version 0.9.0 and
earlier versions, pass the parameter `--skip-update-check` to the programs.

## Version 0.9.0 (2019-04-15)

The program is now able to collect data from the Weatherbit API, too.

## Version 0.8.9 (2019-03-15)

Fix problem with MySQL++ parameter handling on Raspberry Pi.

## Version 0.8.8 (2019-03-14)

The updater will now only perform the most recent update steps that seem to be
required to get from the previous to the current version. Which updates are
required is determined by looking at the current database structure. To avoid
the version detection mechanism and perform all update steps as in previous
program version, use the parameter `--full`.

Furthermore, the C++ standard used during compilation has been raised from
C++11 to C++14. Most compilers should support that by now.

## Version 0.8.7 (2019-03-13)

Switch JSON library from JsonCpp to nlohmann/json. Parsing with that library
only requires ca. 50 % up to 75 % of the time required by JsonCpp.

## Version 0.8.6 (2019-03-02)

Database structure has been adjusted so that weather data and forecast data
where the cloudiness is not known can be stored correctly.

## Version 0.8.5 (2019-02-20)

A new version of the updater fixes some incorrect rain information for forcast
data of OpenWeatherMap where rain was mistakenly set to NULL in the database.
The value is now set to the correct value (zero) instead.

## Version 0.8.4 (2019-02-19)

The updater's update mechanism to version 0.7.0 will now also adjust the wind
direction for current weather entries of OpenWeatherMap that have a non-integer
value as wind direction. (This change should have been part of version 0.7.0,
but it was overlooked, because the change that allowed to parse not only integer
values but also float values as wind direction was introduced in version 0.7.0.)

## Version 0.8.3 (2018-12-25)

Information about precipitation is now split into rain and snow amount.
The Apixu API does not provide that information explicitly, so it is
approximated (currently decided based on temperature).

## Version 0.8.2 (2018-12-22)

The updater will now replaces NULL values in the column `temperature_F` in the
tables `weatherdata` and `forecastdata` with their corresponding values,
calculated from `temperature_K`, where applicable.

This should have been there in version 0.8.0 already, but was missed due to an
oversight.

## Version 0.8.1 (2018-12-12)

Avoid imprecise temperature values due to limited floating-point precision.
Values like e.g. 6.9999 and 1.0003 are now rounded to 7.0 and 1.0, respectively.

## Version 0.8.0 (2018-12-02)

A new program to aid in the creation of weather data collection tasks is added.
It will create task files based on the user's choices.

## Version 0.7.1 (2018-11-30)

The list of predefined paths to look for the configuration file (if it is not
specified on the command line) has been extended by the following values:

* `<HOME>/.wic/wic.conf`
* `<HOME>/.weather-information-collector/wic.conf`
* `<HOME>/.weather-information-collector/weather-information-collector.conf`

The list of predefined paths to look for the task files (if it is not specified
in the configuration file) has been extended by the following values:

* `<HOME>/.wic/task.d`
* `<HOME>/.weather-information-collector/task.d`

In all these cases `<HOME>` stands for the current user's home directory.

## Version 0.7.0 (2018-11-24)

* Collection tasks definitions can now contain a `data` setting that defines the
type of weather data that shall be collected. With that setting the collection
of current weather data, weather forecast data, or the combination of both can
be specified. Note that not all APIs might support every of those data types.
* The weather information collector and the configuration test utility will now
check whether the location data and the data types given in the tasks files are
actually supported by the corresponding API. If not, they will exit with a 
non-zero exit code.
* The update program will now create MySQL tables for forecast data, if they do
  not exist.
* The synchronization tool will now synchronize forecast data, too.

## Version 0.6.7 (2018-07-22)

Use HTTPS instead of HTTP for requests to OpenWeatherMap API.
(Requests to other APIs, i.e. Apixu and DarkSky, used HTTPS from the very
beginning of their implementation in weather-information-collector.)

It would have been there earlier, but since OpenWeatherMap initially did not
support HTTPS for all plans (an example can be seen at e.g.
<https://web.archive.org/web/20150312223316/https://openweathermap.org/price>)
where it still says "No SSL" for the free plan) it first was not implemented to
use HTTPS for weather-information-collector's requests to OpenWeatherMap.
Fortunately that has changed and all plans now support HTTPS for API requests,
so weather-information-collector can now safely use HTTPS for OpenWeatherMap.
With that being said: Thank you, OpenWeatherMap, for providing that additional
safety feature for free plan users, too!

## Version 0.6.6 (2018-07-21)

Due to the fact that some APIs can return quite lengthy JSON data, the database
table for the current weather data has been adjusted so that it now can hold
raw JSON data of up to 16777215 characters per entry instead of only 65535
characters in previous versions.

## Version 0.6.5 (2018-07-20)

Add command line parameter to synchronization tool that allows to set the size
of batch inserts to tweak performance or work around server limitations. The
(default) batch size increases from 25 to 125.

## Version 0.6.4 (2018-07-19)

Improve speed of synchronization program.

## Version 0.6.3 (2018-07-15)

Add utilitly program to synchronize data between two databases.

## Version 0.6.2 (2018-07-12)

Fix wrong return value of `API::getId()` when an error occurred.

## Version 0.6.1 (2018-06-30)

Adjust request limit for Apixu to 10000 requests per month, since they changed
the limit from 5000 to 10000 requests.

## Version 0.6.0 (2018-06-25)

The program is now able to collect data from the DarkSky API, too.

## Version 0.5.7 (2017-09-05)

The update program does not need to load task information or API keys, so the
configuration loading procedure is changed to skip such information for the
update program only.

## Version 0.5.6 (2017-09-03)

Add a new program that performs database structure and data updates for older
database variants (currently only version 0.5.4 and earlier) and brings them
up to data with the current database structure.

## Version 0.5.5 (2017-08-31)

weather-information-collector will now parse the rain amount given in JSON
files, too. This will also be written to the database.

## Version 0.5.4 (2017-08-24)

Add a new program that performs configuration checks only. This way the user
can check whether all configuration files are acceptable without having to start
the data collection.

## Version 0.5.3 (2017-08-24)

**This is the first working version of weather-information-collector.**
It can collect data from two distinct APIs (OpenWeatherMap, Apixu).

Fixes a missing variable initialization issue that resulted in a null pointer
dereference and thus in a segmentation fault.

## Version 0.5.2 (2017-08-23)

Version 0.5.2 fixes a flaw in the detection of overlapping task definitions.

## Version 0.5.1 (2017-08-23)

Version 0.5.1 adds missing information about the configuration file to the
usage / help message.

## Version 0.5 (2017-08-23)

Version 0.5 brings the first (still incomplete) implementation of the data
collection and storage mechanism. It has been completed later in version 0.5.3.
