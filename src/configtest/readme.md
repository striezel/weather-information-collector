# weather-information-collector-configtest

`weather-information-collector-configtest` is a tool that checks the
configuration files of `weather-information-collector` for potential errors.
This includes the core configuration file as well as the task files.

## Usage

```
weather-information-collector-configtest [OPTIONS]

Checks configuration files for potential errors.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -c FILE | --conf FILE  - Sets the file name of the configuration file to use
                           during the program run. If this option is omitted,
                           then the program will search for the configuration
                           in some predefined locations.
  -l | --ignore-limits   - Ignore check for API limits during startup.
```

## Copyright and Licensing

Copyright 2017-2025  Dirk Stolle

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
