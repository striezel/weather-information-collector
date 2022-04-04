#!/bin/bash

# format.sh - script to format C++ files
#
# Copyright (C) 2020  Dirk Stolle
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.

# Find clang-format
FORMATTER=""
formatters=(clang-format-10 clang-format-9 clang-format-8 clang-format-7 clang-format-7.0 clang-format-6.0 clang-format-5.0 clang-format-4.0 clang-format-3.9 clang-format-3.8 clang-format)
for f in ${formatters[@]}
do
  which $f > /dev/null
  if [ $? -eq 0 ]
  then
    FORMATTER=$f
    break
  fi
done

if [ -z $FORMATTER ]
then
  echo "Error: No formatter was found. Try installing clang-format via"
  echo "    apt-get install clang-format"
  echo "or a similar command."
  exit 1
fi
echo "Formatting with $FORMATTER..."

# get directory of this script
SCRIPT_DIR="${BASH_SOURCE%/*}"
cd "$SCRIPT_DIR/.."
if [ $? -ne 0 ]
then
  echo "Could not change directory!"
  exit 2
fi

echo "Searching for header files ..."
FILES=$(find . -type f -name '*.hpp' | grep -v third)
for item in ${FILES[@]}
do
  echo "Formatting $item..."
  $FORMATTER -i "$item"
done
