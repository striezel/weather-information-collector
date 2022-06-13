# Building for Windows with MSYS2

Currently, weather-information-collector does not support native Windows builds
with Microsoft Visual Studio. However, it can be built on Windows systems with
[MSYS2](https://www.msys2.org/). MSYS2 provides some typical GNU / Linux tools
on Windows operating systems. Those can be used to build
weather-information-collector.

## Prerequisites

### MSYS2

First, download the MSYS2 installer from <https://www.msys2.org/#installation>
and run it. After that is done, open "MSYS2 MinGW x64" from the start menu. This
will open a command prompt window. All following steps are done there.

If you haven't done it yet, update any existing base packages by typing

```bash
pacman -Syu
```

into the prompt, then press enter.

### Build tools

To build the weather-information-collector from source you need a C++ compiler,
CMake, the Ninja build system, the cURL library, and the MariaDB library.
pkg-config is required to make it easier to find compiler options for the
installed libraries. Additionally, the program uses Catch (C++ Automated Test
Cases in Headers) to perform some tests.

It also helps to have Git, a distributed version control system, on your build
system to get the latest source code directly from the Git repository.

All of that can usually be installed be typing

```bash
pacman -S git mingw-w64-x86_64-catch mingw-w64-x86_64-curl-winssl mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-libmariadbclient mingw-w64-x86_64-ninja mingw-w64-x86_64-pkg-config
```

into the MSYS2 command prompt.

### Getting the source code

Get the source directly from Git by cloning the Git repository and change to
the directory after the repository is completely cloned:

```bash
git clone https://github.com/striezel/weather-information-collector.git ./wic
cd wic
```

That's it, you should now have the current source code on your machine.

### Build process

The build process is relatively easy, because CMake does all the preparations.
Starting in the root directory of the source, you can do the following steps:

```bash
mkdir build
cd build
cmake ../
cmake --build . -j4
```

It may take a while to build.
