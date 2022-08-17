#!/bin/sh

# Script to test executable when parameters are used in the wrong way.
#
#  Copyright (C) 2022  Dirk Stolle
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

# 1st parameter = executable path
if [ -z "$1" ]
then
  echo "First parameter must be executable file!"
  exit 1
fi

EXECUTABLE="$1"

# parameter --conf / -c given twice
"$EXECUTABLE" --conf foo.conf --conf twice.conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --conf was given twice."
  exit 1
fi

# no file name after parameter --conf
"$EXECUTABLE" --conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --conf had no file name."
  exit 1
fi

# --ignore-limits is given twice
"$EXECUTABLE" --ignore-limits --ignore-limits
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --ignore-limits was given twice."
  exit 1
fi

# unknown parameter
"$EXECUTABLE" --this-is-not-supported
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when an unknown parameter was given."
  exit 1
fi

# Configuration file does not exist / fail to load.
"$EXECUTABLE" --conf this-file-is-missing.conf
if [ $? -ne 2 ]
then
  echo "Executable did not exit with code 2 when configuration file was missing."
  exit 1
fi

exit 0
