# Version history of weather-information-collector

## Version 0.6.4-next (2018-07-xx)

Improve speed of synchronization program.

## Version 0.6.3 (2018-07-15)

Add utilitly program to synchronize data between two databases.

## Version 0.6.2 (2018-07-12)

Fix wrong return value of API::getId() when an error occurred.

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
