#!/bin/bash

# docker-cleanup.sh - script to reduce size of created Docker image
#
# Copyright (C) 2019, 2023  Dirk Stolle
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.

# remove git itself
apt-get purge -y git git-man
# remove stuff for package creation
apt-get purge -y build-essential bzip2 debhelper devscripts fakeroot \
    lsb-release wget
# remove other stuff that might get installed as dependencies
apt-get purge -y gettext gettext-base libpython3-stdlib m4 patch python3 \
    xz-utils
# remove Catch + CMake
apt-get purge catch2 cmake cmake-data
# autoremove what is left
apt-get autoremove -y || true
# clean APT cache
apt-get clean || true
