# weather-information-collector

weather-information-collector will be a tool that collects weather information
from various available ReST API services. It will be highly configurable.

## Build status

[![Build Status](https://travis-ci.org/striezel/weather-information-collector.svg?branch=master)](https://travis-ci.org/striezel/weather-information-collector)

## Building from source

### Prerequisites

**(Not implemented yet)**

To build the scan-tool from source you need a C++ compiler, CMake 2.8 or later,
the cURL library (>=7.17), the JsonCpp library. pkg-config is required to make
it easier to find compiler options for the installed libraries.
It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All of that can usually be installed be typing

    apt-get install cmake g++ git libcurl4-gnutls-dev libjsoncpp-dev pkg-config

or

    yum install cmake gcc-c++ git libcurl-devel jsoncpp-devel pkgconfig

into a root terminal.

### Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

    git clone https://github.com/striezel/weather-information-collector.git ./wic
    cd wic

That's it, you should now have the current source code on your machine.

### Build process

**(Not implemented yet)**

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

    mkdir build
    cd build
    cmake ../
    make -j2

## Copyright and Licensing

Copyright 2017  Dirk Stolle

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
