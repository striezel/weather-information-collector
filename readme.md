# weather-information-collector

weather-information-collector is a tool that collects weather information from
various available ReST API services (currently OpenWeatherMap, Apixu and
DarkSky) and stores the information in a database. Currently only MySQL and
compatible DBMS like MariaDB are supported.

Data collection tasks can be configured individually.
weather-information-collector will check that these collection tasks do not
overlap (i.e. that there are not two tasks which collect the same information
from the same API). It will also check that any configured tasks do not exceed
the request limits imposed by the APIs.

See the [documentation](doc/readme.md) for more information.

## Build status

[![Build Status](https://travis-ci.org/striezel/weather-information-collector.svg?branch=master)](https://travis-ci.org/striezel/weather-information-collector)

## Building from source

### Prerequisites

To build the weather-information-collector from source you need a C++ compiler,
CMake 2.8 or later, the cURL library (>=7.17), the JsonCpp library, the MySQL++
library and Boost's filesystem library. pkg-config is required to make it easier
to find compiler options for the installed libraries. Additionally, the program
uses Catch (C++ Automated Test Cases in Headers) to perform some tests.

It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All of that can usually be installed be typing

    apt-get install catch cmake g++ git libboost-filesystem-dev libcurl4-gnutls-dev libjsoncpp-dev libmysql++-dev pkg-config

or

    yum install catch cmake gcc-c++ git boost-filesystem boost-devel libcurl-devel jsoncpp-devel mysql++-devel pkgconfig

into a root terminal.

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
    make -j4

## Usage

    weather-information-collector [OPTIONS]

    options:
      -? | --help            - shows this help message
      -v | --version         - shows version information
      -c FILE | --conf FILE  - sets the file name of the configuration file to use
                               during the program run. If this option is omitted,
                               then the program will search for the configuration
                               in some predefined locations.
      -l | --ignore-limits   - ignore check for API limits during startup

## Copyright and Licensing

Copyright 2017, 2018  Dirk Stolle

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
