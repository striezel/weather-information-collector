#!/bin/bash

#  create-package.sh - script to create *.deb package from latest Git tag
#
#  Copyright (C) 2019  Dirk Stolle
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

# Gets the latest tag.
TAG="${CI_COMMIT_TAG:-$(git describe | cut -d '-' -f 1)}"
# Version is the same as the tag, just without the leading 'v'.
VERSION=$(echo $TAG | cut -c '2-')
ARCH=$(dpkg --print-architecture)

echo "Info: Tag is $TAG."
echo "Info: Version is $VERSION."
echo "Info: Architecture is $ARCH."

wget -O weather-information-collector_${VERSION}.orig.tar.bz2 https https://gitlab.com/striezel/weather-information-collector/-/archive/${TAG}/weather-information-collector-${TAG}.tar.bz2
tar -x --bzip2 -f weather-information-collector_${VERSION}.orig.tar.bz2
if [[ $? -ne 0 ]]
then
  echo "ERROR: Archive extraction failed!"
  exit 1
fi

mv weather-information-collector-${TAG} weather-information-collector-${VERSION}
if [[ $? -ne 0 ]]
then
  echo "ERROR: Could not move extracted files!"
  exit 1
fi

cd weather-information-collector-${VERSION}
if [[ $? -ne 0 ]]
then
  echo "ERROR: Could not change to extracted directory"
  exit 1
fi

# build the package
debuild -uc -us
if [[ $? -ne 0 ]]
then
  echo "ERROR: Package build failed!"
  exit 1
fi

cd ..
RELEASE=$(lsb_release -cs)
mv weather-information-collector*.deb weather-information-collector_${VERSION}-${RELEASE}_${ARCH}.deb
