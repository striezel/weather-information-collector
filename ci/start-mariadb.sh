#!/bin/bash

# Utility script to start a MariaDB instance in Docker.
#
# Copyright (C) 2021  Dirk Stolle
#
# License: GNU GPL 3+

# bind to port 3307 on 127.0.0.1, because 3306 may be in use
docker run --rm -d -p 127.0.0.1:3307:3306  --name some-mariadb \
 -e MARIADB_ROOT_PASSWORD=my-secret-pw \
 -e MARIADB_DATABASE=weather_information_collector \
 -e MARIADB_USER=ci_user \
 -e MARIADB_PASSWORD=ci_password \
 mariadb:10.6
if [ $? -ne 0 ]
then
  echo Could not start MariaDB container!
  exit 1
fi
docker logs -f some-mariadb
