#!/bin/bash

#  start.sh - script to ease build Docker image + start of container
#
#  Copyright (C) 2019, 2020  Dirk Stolle
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
if [ -z $TAG ]
then
  $TAG=latest
fi

# Image name is first parameter or defaults to "weather".
IMAGE=${1:-weather:$TAG}
# Container name is second parameter or defaults to "weatherbot".
CONTAINER=${2:-weatherbot}
if [ "latest" != "$TAG" ]
then
  CONTAINER=${2:-weatherbot_$TAG}
fi

# Check whether Docker exists / daemon is running and user has permission to
# use it.
docker info > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
  echo "Error: It looks like Docker is not installed or you do not have the"
  echo "required permissions to use Docker."
  exit 1
fi

# Check whether image exists yet. (Redirect STDIN+STDERR to /dev/null.)
docker image inspect "$IMAGE" > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
  echo "Info: Image $IMAGE does not exist, building it now."
  docker build -t "$IMAGE" .
  if [[ $? -ne 0 ]]
  then
    echo "Docker image build process failed!"
    exit 2
  fi
fi

# Check whether a container with the same name already exists. (STDIN and STDERR
# get redirected to /dev/null, because user does not need to see it.)
docker container inspect "$CONTAINER" > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
  echo "Info: Container $CONTAINER does not exist, creating it now."
  docker run --network host -d --name "$CONTAINER" "$IMAGE"
  if [[ $? -ne 0 ]]
  then
    echo "Error: Could not create + start container $CONTAINER!"
    exit 3
  fi
else
  # Container with same name already exists - use it.
  docker start "$CONTAINER"
  if [[ $? -ne 0 ]]
  then
    echo "Error: Could not create + start container $CONTAINER!"
    exit 3
  fi
fi

echo "Info: Container $CONTAINER is running."
exit 0
