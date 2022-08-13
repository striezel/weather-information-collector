# weather-information-collector

`weather-information-collector` is the main program of this collection of
programs and handles the actual weather data collection. Data collection is
executed according to previously created [task files](../../doc/task-files.md)
that specify which data shall be collected. You can use the
[`weather-information-collector-task-creator`](../creator/readme.md) to create
such files, or you can create them manually with the text editor of your choice.

After it is started, the `weather-information-collector` will run indefinitely.
This means it has to be stopped manually (e. g. via Ctrl+C or via the
[`kill` command](https://linux.die.net/man/1/kill) on Linux or via the task
manager on Windows).

## Building from source

See the [readme file in the repository's root directory](../../readme.md) for
information on how to build this program (as well as the other binaries) from
the source code.

## Usage

```
weather-information-collector [OPTIONS]

options:
  -? | --help            - shows this help message
  -v | --version         - shows version information
  -c FILE | --conf FILE  - sets the file name of the configuration file to use
                           during the program run. If this option is omitted,
                           then the program will search for the configuration
                           in some predefined locations.
  -l | --ignore-limits   - ignore check for API limits during startup
  --skip-update-check    - skips the check to determine whether the database
                           is up to date during program startup.
```

## Copyright and Licensing

Copyright 2017-2022  Dirk Stolle

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
