# Version history of weather-information-collector

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
