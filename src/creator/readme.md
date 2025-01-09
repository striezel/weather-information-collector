# weather-information-collector-task-creator

`weather-information-collector-task-creator` is a program that aids in the
creation of collection tasks for `weather-information-collector`. It allows to
specify things like location to collect the data for, the API used for data
collection, type of data and collection interval on the command line and creates
a task file with that information.

Of course, it can be invoked multiple times to create multiple data collection
tasks.

## Usage

```
weather-information-collector-task-creator [OPTIONS]

Creates a task file for weather-information-collector, based on the given user
input and choices.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -c FILE | --conf FILE  - Sets the file name of the configuration file to use
                           during the program run. If this option is omitted,
                           then the program will search for the configuration
                           in some predefined locations.
```

## Copyright and Licensing

Copyright 2018-2025  Dirk Stolle

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
