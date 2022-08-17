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

# parameter --src-conf / -c1 given twice
"$EXECUTABLE" --src-conf foo.conf --src-conf twice.conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --src-conf was given twice."
  exit 1
fi

# no file name after parameter --src-conf
"$EXECUTABLE" --src-conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --src-conf had no file name."
  exit 1
fi

# parameter --dest-conf / -c2 given twice
"$EXECUTABLE" --dest-conf foo.conf --dest-conf twice.conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --dest-conf was given twice."
  exit 1
fi

# no file name after parameter --dest-conf
"$EXECUTABLE" --dest-conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --dest-conf had no file name."
  exit 1
fi

# --batch-size is given twice
"$EXECUTABLE" --batch-size 40 --batch-size 50
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --batch-size was given twice."
  exit 1
fi

# no number after --batch-size
"$EXECUTABLE" --batch-size
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --batch-size was not followed by a number."
  exit 1
fi

# --batch-size followed by anything but a number
"$EXECUTABLE" --batch-size foo
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --batch-size was given an invalid number."
  exit 1
fi

# --batch-size with negative value
"$EXECUTABLE" --batch-size -30
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --batch-size was given a negative number."
  exit 1
fi

# --skip-update-check is given twice
"$EXECUTABLE" --skip-update-check --skip-update-check
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when --skip-update-check was given twice."
  exit 1
fi

# unknown parameter
"$EXECUTABLE" --this-is-not-supported
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when an unknown parameter was given."
  exit 1
fi

# missing source configuration file
"$EXECUTABLE" --dest-conf foo.conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when source configuration file was not specified."
  exit 1
fi

# missing destination configuration file
"$EXECUTABLE" --src-conf foo.conf
if [ $? -ne 1 ]
then
  echo "Executable did not exit with code 1 when destination configuration file was not specified."
  exit 1
fi

# Configuration file does not exist / fail to load.
"$EXECUTABLE" --src-conf this-file-is-missing.conf --dest-conf missing2.conf
if [ $? -ne 2 ]
then
  echo "Executable did not exit with code 2 when configuration file was missing."
  exit 1
fi

exit 0
