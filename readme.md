# weather-information-collector

weather-information-collector is a tool that collects weather information from
various available ReST API services (currently OpenWeatherMap, Open-Meteo,
Weatherbit and Weatherstack) and stores the information in a database.
Currently only MySQL and compatible DBMS like MariaDB are supported.

Data collection tasks can be configured individually.
weather-information-collector will check that these collection tasks do not
overlap (i.e. that there are not two tasks which collect the same information
from the same API). It will also check that any configured tasks do not exceed
the request limits imposed by the APIs.

See the [documentation](doc/readme.md) for more information.

## Usage

weather-information-collector consists of several tools that can perform
different tasks. See the corresponding readme files for usage information:

* [`weather-information-collector`](./src/collector/readme.md) - main tool, does
  the actual weather data collection
* [`weather-information-collector-configtest`](./src/configtest/readme.md) -
  checks configuration files of `weather-information-collector` for errors
* [`weather-information-collector-task-creator`](./src/creator/readme.md) -
  creates configuration files for weather data collection tasks.
* [`weather-information-collector-update`](./src/update/readme.md) - updates the
  database of `weather-information-collector` to the newest version
* [`weather-information-collector-synchronizer`](./src/synchronizer/readme.md) -
  synchronizes data from one `weather-information-collector` database to another

## Build status

* GitHub workflows:
  [![GitHub CI Clang status](https://github.com/striezel/weather-information-collector/workflows/Clang/badge.svg)](https://github.com/striezel/weather-information-collector/actions)
  [![GitHub CI GCC status](https://github.com/striezel/weather-information-collector/workflows/GCC/badge.svg)](https://github.com/striezel/weather-information-collector/actions)
  [![GitHub CI MSYS2 status](https://github.com/striezel/weather-information-collector/workflows/MSYS2/badge.svg)](https://github.com/striezel/weather-information-collector/actions)
* GitLab CI:
[![GitLab pipeline status](https://gitlab.com/striezel/weather-information-collector/badges/master/pipeline.svg)](https://gitlab.com/striezel/weather-information-collector/)

## Building from source

### Prerequisites

To build the weather-information-collector from source you need a C++ compiler
with support for C++17, CMake 3.8 or later, the cURL library (>=7.17), and the
MariaDB library. pkg-config is required to make it easier to find compiler
options for the installed libraries. Additionally, the program uses Catch (C++
Automated Test Cases in Headers) to perform some tests.

It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All of that can usually be installed by typing

    apt-get install catch cmake g++ git libcurl4-gnutls-dev libmariadb-dev pkg-config

or

    yum install catch cmake gcc-c++ git libcurl-devel mariadb-devel pkgconfig

into a root terminal.

_Note: If you do not want to perform a build on bare metal, then try to use
Docker (see `docker/` directory) instead._

### Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

    git clone https://github.com/striezel/weather-information-collector.git ./wic
    cd wic

That's it, you should now have the current source code on your machine.

### Build process

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

    mkdir build
    cd build
    cmake ../
    cmake --build . -j4

### Run with Docker

If you prefer Docker over the native approach, take a look into the `docker/`
directory and refer to the [readme there](./docker/readme.md).

## Building from source on Windows

For information how to build weather-information-collector on Windows, see the
[MSYS2 build instructions](./doc/msys2-build.md).

## Copyright and Licensing

Copyright 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025  Dirk Stolle

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
