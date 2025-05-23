# weather-information-collector-synchronizer

`weather-information-collector-synchronizes` is a tool that can synchronize
weather data of `weather-information-collector` from one database instance to
another. Note that the synchronization process usually requires both database
instances to be up to date and of the same version. (You can use the
[`weather-information-collector-update`](../update/readme.md) program to bring
database instances up to date.)

Depending on the amount of data to synchronize and the speed of the network
connection between both database instances the synchronization process can take
a long time.

## Usage

```
weather-information-collector-synchronizer [OPTIONS]

Synchronizes data from one collector database to another.

options:
  -? | --help            - Shows this help message.
  -v | --version         - Shows version information.
  -c1 FILE | --src-conf FILE
                         - Sets the file name of the configuration file that
                           contains the database connection settings for the
                           source database.
  -c2 FILE | --dest-conf FILE
                         - Sets the file name of the configuration file that
                           contains the database connection settings for the
                           destination database.
  -b N | --batch-size N  - Sets the number of records per batch insert to N.
                           Higher numbers mean increased performance, but it
                           could also result in hitting MySQL's limit for the
                           maximum packet size, called max_allowed_packet.
                           Defaults to 40, if no value is given.
  --skip-update-check    - Skips the check to determine whether the databases
                           are up to date during program startup.
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
