#!/bin/bash

#  cpplint.sh - script to check the syntax of C++ source files
#
#  Copyright (C) 2015, 2017  Dirk Stolle
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

if [[ -z $CXX ]]
then
  # no C++ compiler is set, assume GNU C++ compiler
  CXX=g++
fi

# CFLAGS need to be set for some libraries that require a special include path
# for their headers.
pkg-config --cflags jsoncpp
if [[ $? -eq 0 ]]
then
  CFLAGS=$(pkg-config --cflags jsoncpp)
else
  # just use some standard path for libjsoncpp includes
  echo "Info: Falling back to standard includes for jsoncpp!"
  CFLAGS="-I/usr/include/jsoncpp"
fi
# add includes for libmysql++
CFLAGS="$CFLAGS -I/usr/include/mysql -I/usr/include/mysql++"

# print $CXX version to see which version is used for syntax check
$CXX --version
echo

# find all C++ files and run them through the compilers's syntax check
find ./ \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) -print0 | xargs -0 -i $CXX $CFLAGS -fsyntax-only -Wall -std=c++11 {}
if [[ $? -ne 0 ]]
then
  echo "Some source code files contain syntax errors!"
  echo "You should do something about it."
  echo 'And do it "soon(TM)".'
  exit 1
else
  echo "Syntax seems to be correct."
  echo "Please take this happy smilie with you.  :)"
  exit 0
fi
