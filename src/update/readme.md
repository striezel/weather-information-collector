# weather-information-collector-update

`weather-information-collector-update` is a tool that performs database updates
(schema and data) for the database of `weather-information-collector`. It should
usually be run on an existing database when updating to a newer version of the
`weather-information-collector`.

## Usage

```
weather-information-collector-update [OPTIONS]

Performs database updates for a database that was created for an earlier
version of weather-information-collector.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -c FILE | --conf FILE  - Sets the file name of the configuration file to use
                           during the program run. If this option is omitted,
                           then the program will search for the configuration
                           in some predefined locations.
  -f | --full            - Perform all update steps, even those for older
                           versions that may not be required. This option is
                           off by default, so that only required updates are
                           performed by the update program. Which updates are
                           required is determined by looking at the current
                           database structure.
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
