# weather-information-collector-benchmark

`weather-information-collector-benchmark` is a tool that runs a very basic JSON
parsing benchmarks for different JSON libraries. This is intended purely for
development purposes and is not meant to be used in production.

`weather-information-collector-benchmark` expects an existing database of
`weather-information-collector` populated with some data and uses the data
stored therein to run the benchmark.

## Usage

```
weather-information-collector-benchmark [OPTIONS]

Runs benchmarks for JSON parsing with different libraries.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -c FILE | --conf FILE  - Sets the file name of the configuration file to use
                           during the program run. If this option is omitted,
                           then the program will search for the configuration
                           in some predefined locations.
```

## Copyright and Licensing

Copyright 2019-2024  Dirk Stolle

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
